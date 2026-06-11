/**
 * @file    ec200_core.c
 * @brief   Platform-independent EC200 Quectel 4G LTE modem driver - Core logic
 *
 * All hardware access goes through the port interface registered via Ec200Port_Init().
 * This file contains ZERO platform-specific code.
 */

#include "ec200_core.h"
#include "../../config/project_config.h"
#if MQTT_USE_TLS_CERTS
#include "../cert_Lib/mqtt_certs.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EC200_DBG(msg)  do { \
    const Ec200Port_Interface *_p = Ec200Port_Get(); \
    if (_p && _p->debug_print) _p->debug_print(msg); \
} while (0)

static const Ec200Port_Interface *g_port = NULL;

void Ec200Port_Init(const Ec200Port_Interface *port)
{
    g_port = port;
}

const Ec200Port_Interface* Ec200Port_Get(void)
{
    return g_port;
}

/* Private variables ---------------------------------------------------------*/

EC200_State_t     ec200_state = EC200_STATE_IDLE;
uint8_t           ec200_rx_buf[EC200_RX_BUF_SIZE];
volatile uint16_t ec200_rx_len = 0;
uint32_t          ec200_unix_timestamp = 0;
char              ec200_timestamp_str[32] = {0};
char              ec200_imei[16] = {0};

/* single byte used for interrupt-driven reception */
static uint8_t    s_rx_byte;

/* Private helper: search for a sub-buffer inside a buffer */
static void *ec200_memmem(const void *haystack, size_t haystacklen,
                          const void *needle, size_t needlelen);

static void ec200_delay_ms(uint32_t ms)
{
    if (g_port && g_port->delay_ms)
        g_port->delay_ms(ms);
}

static uint32_t ec200_get_tick_ms(void)
{
    if (g_port && g_port->get_tick_ms)
        return g_port->get_tick_ms();
    return 0U;
}

static void ec200_uart_tx(const uint8_t *data, uint16_t len)
{
    if (g_port && g_port->uart_transmit)
        g_port->uart_transmit(data, len, 0U);
}

/* ======================================================================== */
/*                         Public Functions                                  */
/* ======================================================================== */

void EC200_Init(void)
{
    ec200_state  = EC200_STATE_IDLE;
    ec200_rx_len = 0;
    memset(ec200_rx_buf, 0, EC200_RX_BUF_SIZE);

    if (g_port && g_port->gpio_init)
        g_port->gpio_init();

    if (g_port && g_port->uart_receive_it)
        g_port->uart_receive_it(&s_rx_byte, 1);

    EC200_DBG("[EC200] Init complete\r\n");
}

void EC200_SendCmd(const char *cmd)
{
    ec200_uart_tx((const uint8_t *)cmd, (uint16_t)strlen(cmd));
}

uint8_t EC200_CheckResponse(const char *expected, uint32_t timeout_ms)
{
    if (g_port == NULL || g_port->get_tick_ms == NULL)
        return 0U;

    uint32_t start = ec200_get_tick_ms();
    size_t exp_len = strlen(expected);

    while ((ec200_get_tick_ms() - start) < timeout_ms)
    {
        if (ec200_rx_len == 0)
            continue;
        if (ec200_memmem(ec200_rx_buf, ec200_rx_len, expected, exp_len) != NULL)
            return 1U;
    }
    return 0U;
}

void EC200_ResetRxBuffer(void)
{
    if (g_port && g_port->uart_abort_receive_it)
        g_port->uart_abort_receive_it();
    memset(ec200_rx_buf, 0, EC200_RX_BUF_SIZE);
    ec200_rx_len = 0;
    if (g_port && g_port->uart_receive_it)
        g_port->uart_receive_it(&s_rx_byte, 1);
}

/* ======================================================================== */
/*                     Power Control (EC200 datasheet)                       */
/* ======================================================================== */

void EC200_PowerOn(void)
{
    EC200_DBG("[EC200] Powering on...\r\n");

    if (g_port && g_port->gpio_write) {
        g_port->gpio_write(EC200_GPIO_PWR, EC200_GPIO_HIGH);
        ec200_delay_ms(500);

        EC200_ResetRxBuffer();
        g_port->gpio_write(EC200_GPIO_PWRKEY, EC200_GPIO_LOW);
        ec200_delay_ms(2500);
        g_port->gpio_write(EC200_GPIO_PWRKEY, EC200_GPIO_HIGH);
    }

    EC200_DBG("[EC200] PWRKEY released, waiting for RDY...\r\n");
}

void EC200_PowerOff(void)
{
    EC200_DBG("[EC200] Shutting down modem...\r\n");
    EC200_ResetRxBuffer();
    EC200_SendCmd("AT+QPOWD=1\r\n");

    if (EC200_CheckResponse("POWERED DOWN", 65000))
        EC200_DBG("[EC200] POWERED DOWN received\r\n");
    else
        EC200_DBG("[EC200] POWERED DOWN not received, forcing shutdown\r\n");

    ec200_delay_ms(1000);

    if (g_port && g_port->gpio_write) {
        g_port->gpio_write(EC200_GPIO_PWR, EC200_GPIO_LOW);
        g_port->gpio_write(EC200_GPIO_PWRKEY, EC200_GPIO_HIGH);
    }

    EC200_DBG("[EC200] Modem shut down\r\n");
}

void EC200_Reset(void)
{
    EC200_DBG("[EC200] Resetting modem...\r\n");
    EC200_PowerOff();
    ec200_delay_ms(500);
    EC200_PowerOn();
    EC200_DBG("[EC200] Modem reset complete\r\n");
}

/* ======================================================================== */
/*                   Connection State Machine                                */
/* ======================================================================== */

EC200_Status_t EC200_RunStateMachine(void)
{
    char cmd_buf[128];

    switch (ec200_state)
    {
    /* ------------------------------------------------------------------ */
    case EC200_STATE_IDLE:
        EC200_DBG("[EC200][SM] IDLE -> POWER_ENABLE\r\n");
        ec200_state = EC200_STATE_POWER_ENABLE;
        return EC200_OK;

    /* ------------------------------------------------------------------ */
    case EC200_STATE_POWER_ENABLE:
    {
        /* Check if module is already running — avoid toggling PWRKEY off */
        EC200_ResetRxBuffer();
        EC200_SendCmd("AT\r\n");
        if (EC200_CheckResponse("OK", 1500))
        {
            EC200_DBG("[EC200][SM] Module on, reconnecting MQTT...\r\n");
            EC200_ResetRxBuffer();

            /* Close any existing MQTT session */
            EC200_SendCmd("AT+QMTCLOSE=0\r\n");
            EC200_CheckResponse("OK", 5000);
            EC200_ResetRxBuffer();

            /* Re-apply volatile MQTT settings */
#if MQTT_USE_TLS_CERTS
            EC200_SendCmd("AT+QMTCFG=\"ssl\",0,1,2\r\n");
#else
            EC200_SendCmd("AT+QMTCFG=\"ssl\",0,0\r\n");
#endif
            EC200_CheckResponse("OK", 2000);
            EC200_ResetRxBuffer();

            EC200_SendCmd("AT+QMTCFG=\"version\",0,4\r\n");
            EC200_CheckResponse("OK", 2000);
            EC200_ResetRxBuffer();

            EC200_SendCmd("AT+QMTCFG=\"session\",0,1\r\n");
            EC200_CheckResponse("OK", 2000);
            EC200_ResetRxBuffer();

            snprintf(cmd_buf, sizeof(cmd_buf),
                     "AT+QMTCFG=\"keepalive\",0,%d\r\n", MQTT_KEEP_ALIVE);
            EC200_SendCmd(cmd_buf);
            EC200_CheckResponse("OK", 2000);
            EC200_ResetRxBuffer();

            /* Skip full SSL_CFG on warm reconnect */
            ec200_state = EC200_STATE_MQTT_OPEN;
            return EC200_OK;
        }

        /* Module not responding — full power cycle */
        EC200_PowerOn();
        ec200_state = EC200_STATE_CHECK_READY;
        return EC200_OK;
    }

    /* ------------------------------------------------------------------ */
    case EC200_STATE_CHECK_READY:
    {
        EC200_DBG("[EC200][SM] Waiting for RDY (max 15s)...\r\n");

        if (EC200_CheckResponse("RDY", 15000))
        {
            EC200_DBG("[EC200][SM] RDY received\r\n");
            EC200_ResetRxBuffer();
            ec200_state = EC200_STATE_CHECK_AT;
            return EC200_OK;
        }

        EC200_DBG("[EC200][SM] RDY timeout!\r\n");
        EC200_ResetRxBuffer();
        return EC200_ERROR;
    }

    /* ------------------------------------------------------------------ */
    case EC200_STATE_CHECK_AT:
        EC200_SendCmd("AT\r\n");
        if (EC200_CheckResponse("OK", 2000))
        {
            EC200_DBG("[EC200][SM] AT OK\r\n");
            EC200_ResetRxBuffer();

            /* Clear possible NVM GSM-only setting: set AUTO (LTE+3G+2G) */
            EC200_SendCmd("AT+QCFG=\"nwscanmode\",0,1\r\n");
            EC200_CheckResponse("OK", 3000);
            EC200_ResetRxBuffer();

            ec200_state = EC200_STATE_CHECK_CPIN;
            return EC200_OK;
        }
        EC200_DBG("[EC200][SM] AT timeout!\r\n");
        EC200_ResetRxBuffer();
        return EC200_ERROR;

    /* ------------------------------------------------------------------ */
    case EC200_STATE_CHECK_CPIN:
    {
        uint8_t cpin_ok = 0;
        uint8_t tries;

        for (tries = 0; tries < 5; tries++)
        {
            EC200_ResetRxBuffer();
            EC200_SendCmd("AT+CPIN?\r\n");
            if (EC200_CheckResponse("+CPIN: READY", 5000))
            {
                cpin_ok = 1;
                break;
            }
            ec200_delay_ms(2000);
        }

        if (cpin_ok)
        {
            EC200_DBG("[EC200][SM] SIM ready\r\n");
            EC200_ResetRxBuffer();
            ec200_state = EC200_STATE_CHECK_CREG;
            return EC200_OK;
        }
        EC200_DBG("[EC200][SM] CPIN timeout! SIM not ready.\r\n");
        EC200_ResetRxBuffer();
        return EC200_ERROR;
    }

    /* ------------------------------------------------------------------ */
    case EC200_STATE_CHECK_CREG:
    {
        uint32_t creg_start = ec200_get_tick_ms();
        uint8_t  registered = 0;

        /* Set n=0 so response format is "+CEREG: 0,<stat>" */
        EC200_ResetRxBuffer();
        EC200_SendCmd("AT+CREG=0\r\n");
        EC200_CheckResponse("OK", 2000);

        EC200_DBG("[EC200][SM] Waiting for network registration (max 60s)...\r\n");

        while ((ec200_get_tick_ms() - creg_start) < 60000U)
        {
            const char *p;
            EC200_ResetRxBuffer();
            EC200_SendCmd("AT+CREG?\r\n");
            EC200_CheckResponse("+CREG:", 3000);

            EC200_DBG("[EC200][CREG] RX: ");
            EC200_DBG((ec200_rx_len > 0) ? (const char *)ec200_rx_buf : "(empty)");
            EC200_DBG("\r\n");

            /* Independent of n parameter: stat=1 (home) or stat=5 (roaming) */
            p = strstr((char *)ec200_rx_buf, "+CREG:");
            if (p != NULL && (strstr(p, ",1") != NULL || strstr(p, ",5") != NULL))
            {
                registered = 1;
                break;
            }
            ec200_delay_ms(2000);
        }

        if (registered)
        {
            EC200_DBG("[EC200][SM] Registered on network\r\n");
            EC200_ResetRxBuffer();
            ec200_state = EC200_STATE_SET_APN;
            return EC200_OK;
        }
        else
        {
            EC200_DBG("[EC200][SM] CREG 60s timeout! Resetting modem...\r\n");
            EC200_ResetRxBuffer();
            ec200_state = EC200_STATE_RESET;
            return EC200_OK;
        }
    }

    /* ------------------------------------------------------------------ */
    case EC200_STATE_SET_APN:
        snprintf(cmd_buf, sizeof(cmd_buf),
                 "AT+QICSGP=%d,1,\"%s\",\"%s\",\"%s\",%d\r\n",
                 CELLULAR_PDP_CONTEXT, CELLULAR_APN,
                 CELLULAR_APN_USER, CELLULAR_APN_PASS, CELLULAR_APN_AUTH);
        EC200_SendCmd(cmd_buf);
        if (EC200_CheckResponse("OK", 5000))
        {
            EC200_DBG("[EC200][SM] APN configured\r\n");
            EC200_ResetRxBuffer();
            ec200_state = EC200_STATE_ENABLE_QIACT;
            return EC200_OK;
        }
        EC200_DBG("[EC200][SM] APN configuration failed!\r\n");
        EC200_ResetRxBuffer();
        return EC200_ERROR;

    /* ------------------------------------------------------------------ */
    case EC200_STATE_ENABLE_QIACT:
    {
        /* Check current context state */
        EC200_ResetRxBuffer();
        EC200_SendCmd("AT+QIACT?\r\n");
        EC200_CheckResponse("OK", 5000);
        EC200_DBG("[EC200][QIACT] Status: ");
        EC200_DBG((ec200_rx_len > 0) ? (const char *)ec200_rx_buf : "(empty)");
        EC200_DBG("\r\n");

        if (strstr((char *)ec200_rx_buf, "+QIACT: 1,1") != NULL)
        {
            EC200_DBG("[EC200][SM] PDP context already active, continuing...\r\n");
            EC200_ResetRxBuffer();
            ec200_state = EC200_STATE_SSL_CFG;
            return EC200_OK;
        }

        /* --- Pre-QIACT diagnostics --- */
        EC200_ResetRxBuffer();
        EC200_SendCmd("AT+CEREG?\r\n");
        EC200_CheckResponse("OK", 3000);
        EC200_DBG("[EC200][DIAG] CEREG : ");
        EC200_DBG((ec200_rx_len > 0) ? (const char *)ec200_rx_buf : "(empty)");
        EC200_DBG("\r\n");

        EC200_ResetRxBuffer();
        EC200_SendCmd("AT+CGATT?\r\n");
        EC200_CheckResponse("OK", 3000);
        EC200_DBG("[EC200][DIAG] CGATT : ");
        EC200_DBG((ec200_rx_len > 0) ? (const char *)ec200_rx_buf : "(empty)");
        EC200_DBG("\r\n");

        EC200_ResetRxBuffer();
        EC200_SendCmd("AT+QNWINFO\r\n");
        EC200_CheckResponse("OK", 3000);
        EC200_DBG("[EC200][DIAG] NWINFO: ");
        EC200_DBG((ec200_rx_len > 0) ? (const char *)ec200_rx_buf : "(empty)");
        EC200_DBG("\r\n");

        EC200_ResetRxBuffer();
        EC200_SendCmd("AT+QICSGP=1\r\n");
        EC200_CheckResponse("OK", 3000);
        EC200_DBG("[EC200][DIAG] QICSGP: ");
        EC200_DBG((ec200_rx_len > 0) ? (const char *)ec200_rx_buf : "(empty)");
        EC200_DBG("\r\n");

        /* Force deactivate first, then activate clean */
        EC200_DBG("[EC200][SM] Deactivating context...\r\n");
        EC200_ResetRxBuffer();
        EC200_SendCmd("AT+QIDEACT=1\r\n");
        EC200_CheckResponse("OK", 40000);  /* ignore error */
        ec200_delay_ms(1000);

        /* Activate — break immediately on OK or ERROR, do not wait full timeout */
        EC200_DBG("[EC200][SM] Activating PDP context (max 150s)...\r\n");
        EC200_ResetRxBuffer();
        EC200_SendCmd("AT+QIACT=1\r\n");
        {
            uint32_t qiact_t = ec200_get_tick_ms();
            uint8_t  qiact_ok = 0;

            while ((ec200_get_tick_ms() - qiact_t) < 150000U)
            {
                if (ec200_rx_len == 0)
                    continue;
                if (strstr((char *)ec200_rx_buf, "OK") != NULL)
                {
                    EC200_DBG("[EC200][SM] PDP context active\r\n");
                    qiact_ok = 1;
                    break;
                }
                if (strstr((char *)ec200_rx_buf, "ERROR") != NULL)
                {
                    EC200_DBG("[EC200][SM] QIACT ERROR! RX: ");
                    EC200_DBG((const char *)ec200_rx_buf);
                    EC200_DBG("\r\n");
                    break;
                }
            }

            if (!qiact_ok)
                EC200_DBG("[EC200][SM] QIACT failed.\r\n");

            EC200_ResetRxBuffer();
            ec200_state = qiact_ok ? EC200_STATE_SSL_CFG : EC200_STATE_RESET;
            return EC200_OK;
        }
    }

    /* ------------------------------------------------------------------ */
    case EC200_STATE_SSL_CFG:
    {
        /* MQTT SSL mode */
        EC200_ResetRxBuffer();
#if MQTT_USE_TLS_CERTS
        EC200_SendCmd("AT+QMTCFG=\"ssl\",0,1,2\r\n");
#else
        EC200_SendCmd("AT+QMTCFG=\"ssl\",0,0\r\n");
#endif
        if (!EC200_CheckResponse("OK", 5000))
        {
            EC200_DBG("[EC200][SSL] MQTT SSL cfg ERROR!\r\n");
            EC200_ResetRxBuffer();
            return EC200_ERROR;
        }
        EC200_DBG("[EC200][SSL] MQTT SSL mod OK\r\n");
        EC200_ResetRxBuffer();

        /* MQTT 3.1.1 */
        EC200_SendCmd("AT+QMTCFG=\"version\",0,4\r\n");
        if (!EC200_CheckResponse("OK", 5000))
        {
            EC200_DBG("[EC200][SSL] MQTT version cfg ERROR!\r\n");
            EC200_ResetRxBuffer();
            return EC200_ERROR;
        }
        EC200_ResetRxBuffer();

        /* Clean session */
        EC200_SendCmd("AT+QMTCFG=\"session\",0,1\r\n");
        if (!EC200_CheckResponse("OK", 5000))
        {
            EC200_DBG("[EC200][SSL] MQTT session cfg ERROR!\r\n");
            EC200_ResetRxBuffer();
            return EC200_ERROR;
        }
        EC200_ResetRxBuffer();

        /* Keep-alive 60s */
        snprintf(cmd_buf, sizeof(cmd_buf),
                 "AT+QMTCFG=\"keepalive\",0,%d\r\n", MQTT_KEEP_ALIVE);
        EC200_SendCmd(cmd_buf);
        if (!EC200_CheckResponse("OK", 5000))
        {
            EC200_DBG("[EC200][SSL] MQTT keepalive cfg ERROR!\r\n");
            EC200_ResetRxBuffer();
            return EC200_ERROR;
        }
        EC200_ResetRxBuffer();

#if MQTT_USE_TLS_CERTS
        char ssl_cmd[80];

        /* Delete old certificate files */
        EC200_SendCmd("AT+QFDEL=\"cacert.pem\"\r\n");
        EC200_CheckResponse("OK", 3000);
        EC200_ResetRxBuffer();

        EC200_SendCmd("AT+QFDEL=\"client.pem\"\r\n");
        EC200_CheckResponse("OK", 3000);
        EC200_ResetRxBuffer();

        EC200_SendCmd("AT+QFDEL=\"user_key.pem\"\r\n");
        EC200_CheckResponse("OK", 3000);
        EC200_ResetRxBuffer();

        /* ---------- Upload CA certificate ---------- */
        snprintf(ssl_cmd, sizeof(ssl_cmd),
                 "AT+QFUPL=\"cacert.pem\",%u,100\r\n", (unsigned)MqttCerts_GetRootCALen());
        EC200_SendCmd(ssl_cmd);
        {
            uint32_t _t = ec200_get_tick_ms();
            uint8_t  _got = 0;
            while ((ec200_get_tick_ms() - _t) < 5000U)
            {
                if (ec200_rx_len > 0)
                {
                    if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "CONNECT", 7) != NULL)
                    { _got = 1; break; }
                    if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "+CME ERROR: 407", 15) != NULL)
                    { _got = 2; break; }
                }
            }
            if (_got == 2)
            {
                EC200_DBG("[EC200][SSL] CA cert already present\r\n");
                EC200_ResetRxBuffer();
            }
            else if (_got == 1)
            {
                EC200_ResetRxBuffer();
                EC200_SendCmd(MqttCerts_GetRootCA());
                if (!EC200_CheckResponse("+QFUPL:", 10000))
                {
                    EC200_DBG("[EC200][SSL] CA cert upload ERROR!\r\n");
                    EC200_ResetRxBuffer();
                    return EC200_ERROR;
                }
                EC200_DBG("[EC200][SSL] CA cert uploaded\r\n");
                EC200_ResetRxBuffer();
            }
            else
            {
                EC200_DBG("[EC200][SSL] CA cert CONNECT/407 not received!\r\n");
                EC200_ResetRxBuffer();
                return EC200_ERROR;
            }
        }

        /* ---------- Upload client certificate ---------- */
        snprintf(ssl_cmd, sizeof(ssl_cmd),
                 "AT+QFUPL=\"client.pem\",%u,100\r\n", (unsigned)MqttCerts_GetClientCertLen());
        EC200_SendCmd(ssl_cmd);
        {
            uint32_t _t = ec200_get_tick_ms();
            uint8_t  _got = 0;
            while ((ec200_get_tick_ms() - _t) < 5000U)
            {
                if (ec200_rx_len > 0)
                {
                    if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "CONNECT", 7) != NULL)
                    { _got = 1; break; }
                    if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "+CME ERROR: 407", 15) != NULL)
                    { _got = 2; break; }
                }
            }
            if (_got == 2)
            {
                EC200_DBG("[EC200][SSL] Client cert already present\r\n");
                EC200_ResetRxBuffer();
            }
            else if (_got == 1)
            {
                EC200_ResetRxBuffer();
                EC200_SendCmd(MqttCerts_GetClientCert());
                if (!EC200_CheckResponse("+QFUPL:", 10000))
                {
                    EC200_DBG("[EC200][SSL] Client cert upload ERROR!\r\n");
                    EC200_ResetRxBuffer();
                    return EC200_ERROR;
                }
                EC200_DBG("[EC200][SSL] Client cert uploaded\r\n");
                EC200_ResetRxBuffer();
            }
            else
            {
                EC200_DBG("[EC200][SSL] Client cert CONNECT/407 not received!\r\n");
                EC200_ResetRxBuffer();
                return EC200_ERROR;
            }
        }

        /* ---------- Upload private key ---------- */
        snprintf(ssl_cmd, sizeof(ssl_cmd),
                 "AT+QFUPL=\"user_key.pem\",%u,100\r\n", (unsigned)MqttCerts_GetPrivateKeyLen());
        EC200_SendCmd(ssl_cmd);
        {
            uint32_t _t = ec200_get_tick_ms();
            uint8_t  _got = 0;
            while ((ec200_get_tick_ms() - _t) < 5000U)
            {
                if (ec200_rx_len > 0)
                {
                    if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "CONNECT", 7) != NULL)
                    { _got = 1; break; }
                    if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "+CME ERROR: 407", 15) != NULL)
                    { _got = 2; break; }
                }
            }
            if (_got == 2)
            {
                EC200_DBG("[EC200][SSL] Private key already present\r\n");
                EC200_ResetRxBuffer();
            }
            else if (_got == 1)
            {
                EC200_ResetRxBuffer();
                EC200_SendCmd(MqttCerts_GetPrivateKey());
                if (!EC200_CheckResponse("+QFUPL:", 10000))
                {
                    EC200_DBG("[EC200][SSL] Private key upload ERROR!\r\n");
                    EC200_ResetRxBuffer();
                    return EC200_ERROR;
                }
                EC200_DBG("[EC200][SSL] Private key uploaded\r\n");
                EC200_ResetRxBuffer();
            }
            else
            {
                EC200_DBG("[EC200][SSL] Private key CONNECT/407 not received!\r\n");
                EC200_ResetRxBuffer();
                return EC200_ERROR;
            }
        }

        /* SSL context configuration */
        EC200_SendCmd("AT+QSSLCFG=\"cacert\",2,\"cacert.pem\"\r\n");
        if (!EC200_CheckResponse("OK", 5000))
        { EC200_DBG("[EC200][SSL] cacert cfg ERROR!\r\n"); EC200_ResetRxBuffer(); return EC200_ERROR; }
        EC200_ResetRxBuffer();

        EC200_SendCmd("AT+QSSLCFG=\"clientcert\",2,\"client.pem\"\r\n");
        if (!EC200_CheckResponse("OK", 5000))
        { EC200_DBG("[EC200][SSL] clientcert cfg ERROR!\r\n"); EC200_ResetRxBuffer(); return EC200_ERROR; }
        EC200_ResetRxBuffer();

        EC200_SendCmd("AT+QSSLCFG=\"clientkey\",2,\"user_key.pem\"\r\n");
        if (!EC200_CheckResponse("OK", 5000))
        { EC200_DBG("[EC200][SSL] clientkey cfg ERROR!\r\n"); EC200_ResetRxBuffer(); return EC200_ERROR; }
        EC200_ResetRxBuffer();

        EC200_SendCmd("AT+QSSLCFG=\"seclevel\",2,2\r\n");
        if (!EC200_CheckResponse("OK", 5000))
        { EC200_DBG("[EC200][SSL] seclevel cfg ERROR!\r\n"); EC200_ResetRxBuffer(); return EC200_ERROR; }
        EC200_ResetRxBuffer();

        EC200_SendCmd("AT+QSSLCFG=\"sslversion\",2,4\r\n");
        if (!EC200_CheckResponse("OK", 5000))
        { EC200_DBG("[EC200][SSL] sslversion cfg ERROR!\r\n"); EC200_ResetRxBuffer(); return EC200_ERROR; }
        EC200_ResetRxBuffer();

        EC200_SendCmd("AT+QSSLCFG=\"ciphersuite\",2,0xFFFF\r\n");
        if (!EC200_CheckResponse("OK", 5000))
        { EC200_DBG("[EC200][SSL] ciphersuite cfg ERROR!\r\n"); EC200_ResetRxBuffer(); return EC200_ERROR; }
        EC200_ResetRxBuffer();

        EC200_SendCmd("AT+QSSLCFG=\"ignorelocaltime\",2,1\r\n");
        if (!EC200_CheckResponse("OK", 5000))
        { EC200_DBG("[EC200][SSL] ignorelocaltime cfg ERROR!\r\n"); EC200_ResetRxBuffer(); return EC200_ERROR; }
        EC200_ResetRxBuffer();

        EC200_SendCmd("AT+QSSLCFG=\"sni\",2,1\r\n");
        if (!EC200_CheckResponse("OK", 5000))
        { EC200_DBG("[EC200][SSL] sni cfg ERROR!\r\n"); EC200_ResetRxBuffer(); return EC200_ERROR; }
        EC200_ResetRxBuffer();

        EC200_DBG("[EC200][SSL] TLS certificate configuration complete\r\n");
#endif /* MQTT_USE_TLS_CERTS */

        EC200_DBG("[EC200][SSL] MQTT configuration complete\r\n");
        ec200_state = EC200_STATE_MQTT_OPEN;
        return EC200_OK;
    }

    /* ------------------------------------------------------------------ */
    case EC200_STATE_MQTT_OPEN:
    {
        uint8_t open_ok = 0U;
        const uint8_t MAX_OPEN_RETRIES = 3U;

        for (uint8_t attempt = 0U; attempt < MAX_OPEN_RETRIES && !open_ok; attempt++)
        {
            if (attempt > 0U)
            {
                EC200_ResetRxBuffer();
                EC200_SendCmd("AT+QMTCLOSE=0\r\n");
                EC200_CheckResponse("OK", 5000);
                EC200_ResetRxBuffer();
                ec200_delay_ms(2000);
            }

            EC200_ResetRxBuffer();
            snprintf(cmd_buf, sizeof(cmd_buf),
                     "AT+QMTOPEN=0,\"%s\",%u\r\n",
                     MQTT_BROKER_HOST, (unsigned)MQTT_BROKER_PORT);
            EC200_SendCmd(cmd_buf);

            if (!EC200_CheckResponse("OK", 5000))
                continue;

            EC200_DBG("[EC200][SM] MQTT Open: OK, waiting for URC...\r\n");

            /* Wait for async URC: +QMTOPEN: 0,<result> */
            uint32_t urc_start = ec200_get_tick_ms();
            while ((ec200_get_tick_ms() - urc_start) < 60000U)
            {
                if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "+QMTOPEN:", 9) != NULL)
                {
                    EC200_DBG("[EC200][SM] QMTOPEN URC: ");
                    EC200_DBG((const char *)ec200_rx_buf);
                    EC200_DBG("\r\n");

                    /* result=0 only → opened successfully. Any other code is failure.
                     * 1=wrong param, 2=client occupied, 3=PDP failed,
                     * 4=DNS fail, 5=network disconnected */
                    if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "+QMTOPEN: 0,0", 13) != NULL)
                    {
                        open_ok = 1U;
                    }
                    else
                    {
                        EC200_DBG("[EC200][SM] QMTOPEN error code! BUF: ");
                        EC200_DBG((const char *)ec200_rx_buf);
                        EC200_DBG("\r\n");
                    }
                    break;
                }
            }

            if (!open_ok && (ec200_get_tick_ms() - urc_start) >= 60000U)
            {
                /* 60s passed, no URC at all — log buffer content */
                EC200_DBG("[EC200][SM] QMTOPEN 60s timeout! BUF: ");
                EC200_DBG((ec200_rx_len > 0) ? (const char *)ec200_rx_buf : "(empty)");
                EC200_DBG("\r\n");
            }
        }

        if (open_ok)
        {
            EC200_DBG("[EC200][SM] MQTT Open successful\r\n");
            EC200_ResetRxBuffer();
            ec200_state = EC200_STATE_MQTT_CONNECT;
            return EC200_OK;
        }
        EC200_DBG("[EC200][SM] MQTT Open failed!\r\n");
        EC200_ResetRxBuffer();
        /* Module still running — perform proper RESET instead of PWRKEY pulse */
        ec200_state = EC200_STATE_RESET;
        return EC200_OK;
    }

    /* ------------------------------------------------------------------ */
    case EC200_STATE_MQTT_CONNECT:
    {
        ec200_delay_ms(100);
        snprintf(cmd_buf, sizeof(cmd_buf), "AT+QMTCONN=0,\"%s\"\r\n", MQTT_CLIENT_ID);
        EC200_SendCmd(cmd_buf);
        EC200_DBG("[EC200][SM] QMTCONN sent (max 60s)...\r\n");

        uint32_t _t = ec200_get_tick_ms();
        uint8_t  _got = 0;
        while ((ec200_get_tick_ms() - _t) < 60000U)
        {
            if (ec200_rx_len > 0)
            {
                if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "+QMTCONN:", 9) != NULL)
                { _got = 1; break; }
                if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "+QMTSTAT:", 9) != NULL)
                { _got = 2; break; }
            }
        }
        if (_got == 0)
        {
            ec200_delay_ms(1000);
            if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "+QMTCONN:", 9) != NULL)
                _got = 1;
            else if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "+QMTSTAT:", 9) != NULL)
                _got = 2;
        }

        if (_got == 1)
        {
            EC200_DBG("[EC200][SM] QMTCONN RSP: ");
            EC200_DBG((const char *)ec200_rx_buf);
            EC200_DBG("\r\n");

            /* +QMTCONN: <clientID>,<result>[,<ret_code>]
             * result=0 AND ret_code=0 → accepted
             * result=0, ret_code=1..5 → CONNACK rejected by broker */
            if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "+QMTCONN: 0,0,0", 15) != NULL ||
                ec200_memmem(ec200_rx_buf, ec200_rx_len, "+QMTCONN: 0,0\r", 14) != NULL)
            {
                EC200_DBG("[EC200][SM] MQTT Connect successful\r\n");
                EC200_ResetRxBuffer();

                snprintf(cmd_buf, sizeof(cmd_buf),
                         "AT+QIDNSGIP=1,\"%s\"\r\n", MQTT_BROKER_HOST);
                EC200_SendCmd(cmd_buf);
                if (EC200_CheckResponse("+QIURC: \"dnsgip\"", 10000))
                {
                    EC200_DBG("[EC200][SM] DNS RSP: ");
                    EC200_DBG((const char *)ec200_rx_buf);
                    EC200_DBG("\r\n");
                }
                else
                {
                    EC200_DBG("[EC200][SM] DNS resolution failed!\r\n");
                }
                EC200_ResetRxBuffer();

                ec200_state = EC200_STATE_GET_TIMESTAMP;
                return EC200_OK;
            }
            EC200_DBG("[EC200][SM] MQTT CONNACK rejected! (see RSP above)\r\n");
            EC200_ResetRxBuffer();
            return EC200_ERROR;
        }
        else if (_got == 2)
        {
            EC200_DBG("[EC200][SM] MQTT Connect rejected!\r\n");
            EC200_ResetRxBuffer();
            return EC200_ERROR;
        }
        EC200_DBG("[EC200][SM] MQTT Connect timeout!\r\n");
        EC200_ResetRxBuffer();
        return EC200_ERROR;
    }

    /* ------------------------------------------------------------------ */
    case EC200_STATE_GET_TIMESTAMP:
    {
        EC200_Status_t ts_ret = EC200_GetTimestamp();
        /* Always proceed to IMEI regardless of timestamp result */
        ec200_state = EC200_STATE_GET_IMEI;
        return EC200_OK;
    }

    /* ------------------------------------------------------------------ */
    case EC200_STATE_GET_IMEI:
    {
        EC200_Status_t imei_ret = EC200_GetIMEI();
        if (imei_ret != EC200_OK)
            snprintf(ec200_imei, sizeof(ec200_imei), "unknown");
        ec200_state = EC200_STATE_MQTT_PUBLISH;
        return EC200_OK;
    }

    /* ------------------------------------------------------------------ */
    case EC200_STATE_MQTT_PUBLISH:
        EC200_DBG("[EC200][SM] MQTT_PUBLISH ready, ready to send data\r\n");
        return EC200_OK;

    /* ------------------------------------------------------------------ */
    case EC200_STATE_MQTT_DISCONNECT:
        EC200_SendCmd("AT+QMTDISC=0\r\n");
        if (EC200_CheckResponse("+QMTDISC: 0,0", 5000))
            EC200_DBG("[EC200][SM] MQTT Disconnect successful\r\n");
        else
            EC200_DBG("[EC200][SM] MQTT Disconnect timeout\r\n");
        EC200_ResetRxBuffer();
        ec200_state = EC200_STATE_POWER_OFF;
        return EC200_OK;

    /* ------------------------------------------------------------------ */
    case EC200_STATE_POWER_OFF:
        EC200_PowerOff();
        ec200_state = EC200_STATE_DONE;
        return EC200_OK;

    /* ------------------------------------------------------------------ */
    case EC200_STATE_RESET:
        EC200_DBG("[EC200][SM] RESET: restarting modem...\r\n");
        EC200_PowerOff();
        ec200_delay_ms(500);
        EC200_PowerOn();
        ec200_state = EC200_STATE_CHECK_READY;
        return EC200_OK;

    /* ------------------------------------------------------------------ */
    case EC200_STATE_DONE:
        EC200_DBG("[EC200][SM] Operation complete\r\n");
        return EC200_OK;

    case EC200_STATE_ERROR:
        EC200_DBG("[EC200][SM] ERROR state!\r\n");
        return EC200_ERROR;

    default:
        return EC200_ERROR;
    }
}

/* ======================================================================== */
/*                       MQTT Publish                                        */
/* ======================================================================== */

EC200_Status_t EC200_PublishSensorData(const char *topic, const char *data)
{
    char cmd_buf[160];
    size_t payload_len;
    char *pub_ptr;

    if (topic == NULL || data == NULL)
        return EC200_ERROR;

    payload_len = strlen(data);
    if (payload_len == 0U || payload_len > 512U)
        return EC200_ERROR;

    snprintf(cmd_buf, sizeof(cmd_buf),
             "AT+QMTPUB=0,0,0,0,\"%s\",%u\r\n",
             topic, (unsigned)payload_len);

    EC200_ResetRxBuffer();
    EC200_SendCmd(cmd_buf);

    if (!EC200_CheckResponse(">", 5000))
    {
        EC200_DBG("[EC200][PUB] No prompt! BUF: ");
        EC200_DBG(ec200_rx_len > 0 ? (const char *)ec200_rx_buf : "(empty)");
        EC200_DBG("\r\n");
        { uint8_t ctrlz = 0x1A; ec200_uart_tx(&ctrlz, 1); }
        ec200_delay_ms(500);
        EC200_ResetRxBuffer();
        return EC200_ERROR;
    }

    EC200_ResetRxBuffer();
    ec200_delay_ms(50);
    EC200_SendCmd(data);

    if (!EC200_CheckResponse("+QMTPUB:", 15000))
    {
        EC200_DBG("[EC200][PUB] Timeout! BUF: ");
        EC200_DBG(ec200_rx_len > 0 ? (const char *)ec200_rx_buf : "(empty)");
        EC200_DBG("\r\n");
        { uint8_t ctrlz = 0x1A; ec200_uart_tx(&ctrlz, 1); }
        ec200_delay_ms(1000);
        EC200_ResetRxBuffer();
        return EC200_ERROR;
    }

    /* +QMTPUB: found — wait for the full line to arrive before parsing */
    ec200_delay_ms(300);

    /* Success: +QMTPUB: 0,0,0  (result field = 0 or 1) */
    if (ec200_memmem(ec200_rx_buf, ec200_rx_len, "+QMTPUB: 0,0,0", 14) != NULL ||
        ec200_memmem(ec200_rx_buf, ec200_rx_len, "+QMTPUB: 0,0,1", 14) != NULL)
    {
        EC200_ResetRxBuffer();
        return EC200_OK;
    }

    pub_ptr = strstr((char *)ec200_rx_buf, "+QMTPUB:");
    if (pub_ptr != NULL)
    {
        EC200_DBG("[EC200][PUB] Error: ");
        EC200_DBG(pub_ptr);
        EC200_DBG("\r\n");
    }

    EC200_ResetRxBuffer();
    return EC200_ERROR;
}

/* ======================================================================== */
/*                       Timestamp & IMEI                                    */
/* ======================================================================== */

EC200_Status_t EC200_GetTimestamp(void)
{
    EC200_SendCmd("AT+QLTS=1\r\n");

    if (EC200_CheckResponse("+QLTS:", 5000))
    {
        char *ts_start = strstr((char *)ec200_rx_buf, "+QLTS: \"");
        if (ts_start != NULL)
        {
            ts_start += 8;
            int yr, mo, dy, hh, mm, ss;
            if (sscanf(ts_start, "%d/%d/%d,%d:%d:%d", &yr, &mo, &dy, &hh, &mm, &ss) == 6)
            {
                const uint16_t days_before_month[] =
                    {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
                int y = yr - 1970;
                uint32_t days = (uint32_t)(y * 365 + (y + 1) / 4);
                days += days_before_month[mo] + (uint32_t)(dy - 1);
                if (mo > 2 && ((yr % 4 == 0 && yr % 100 != 0) || (yr % 400 == 0)))
                    days += 1U;
                ec200_unix_timestamp = days * 86400UL +
                    (uint32_t)hh * 3600UL + (uint32_t)mm * 60UL + (uint32_t)ss;

                hh += 3;
                if (hh >= 24) { hh -= 24; dy += 1; }
                snprintf(ec200_timestamp_str, sizeof(ec200_timestamp_str),
                         "%04d/%02d/%02d,%02d:%02d:%02d", yr, mo, dy, hh, mm, ss);

                EC200_ResetRxBuffer();
                return EC200_OK;
            }
        }
        EC200_ResetRxBuffer();
        return EC200_ERROR;
    }
    EC200_ResetRxBuffer();
    return EC200_ERROR;
}

EC200_Status_t EC200_GetIMEI(void)
{
    EC200_ResetRxBuffer();
    EC200_SendCmd("AT+GSN\r\n");

    if (EC200_CheckResponse("OK", 3000))
    {
        char *p = (char *)ec200_rx_buf;
        char *end = NULL;

        while (*p != '\0' && (*p < '0' || *p > '9'))
            p++;
        end = p;
        while (*end >= '0' && *end <= '9')
            end++;

        uint8_t imei_len = (uint8_t)(end - p);
        if (imei_len == 15U)
        {
            memcpy(ec200_imei, p, 15U);
            ec200_imei[15] = '\0';
            EC200_ResetRxBuffer();
            return EC200_OK;
        }
    }
    EC200_ResetRxBuffer();
    return EC200_ERROR;
}

/* ======================================================================== */
/*              UART RX Interrupt Callback                                   */
/* ======================================================================== */

void EC200_UART_RxCallback(void)
{
    if (ec200_rx_len < EC200_RX_BUF_SIZE)
    {
        ec200_rx_buf[ec200_rx_len] = s_rx_byte;
        ec200_rx_len++;
    }
    if (g_port && g_port->uart_receive_it)
        g_port->uart_receive_it(&s_rx_byte, 1);
}

/* ======================================================================== */
/*                        Private Helpers                                    */
/* ======================================================================== */

static void *ec200_memmem(const void *haystack, size_t haystacklen,
                          const void *needle, size_t needlelen)
{
    if (needlelen == 0)
        return (void *)haystack;
    if (haystacklen < needlelen)
        return NULL;

    const uint8_t *h = (const uint8_t *)haystack;
    for (size_t i = 0; i <= haystacklen - needlelen; i++)
    {
        if (h[i] == ((const uint8_t *)needle)[0])
        {
            if (memcmp(&h[i], needle, needlelen) == 0)
                return (void *)&h[i];
        }
    }
    return NULL;
}
