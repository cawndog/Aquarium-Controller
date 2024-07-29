#ifndef MAIL_CLIENT_H
#define MAIL_CLIENT_H
/*
 * SMTP email client
 *
 * Adapted from the `ssl_mail_client` example in mbedtls.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2015-2021 Espressif Systems (Shanghai) CO LTD
 */
#include "Arduino.h"
#include "Environment.h"
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include <mbedtls/base64.h>
#include <sys/param.h>

/* Constants that are configured in Environment.h
#define MAIL_SERVER         CONFIG_SMTP_SERVER
#define MAIL_PORT           CONFIG_SMTP_PORT_NUMBER
#define SENDER_MAIL         CONFIG_SMTP_SENDER_MAIL
#define SENDER_PASSWORD     CONFIG_SMTP_SENDER_PASSWORD
#define RECIPIENT_MAIL      CONFIG_SMTP_RECIPIENT_MAIL
*/

#define TASK_STACK_SIZE     (8 * 1024)

/**
 * Root cert for smtp.googlemail.com, taken from server_root_cert.pem
 *
 * The PEM file was extracted from the output of this command:
 * openssl s_client -showcerts -connect smtp.googlemail.com:587 -starttls smtp
 *
 * The CA root cert is the last cert given in the chain of certs.
 *
 * To embed it in the app binary, the PEM file is named
 * in the component.mk COMPONENT_EMBED_TXTFILES variable.
 */
//Embeded server_root_cert.pem
extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");
//Embeded green_fish.png
extern const uint8_t green_fish_png_start[] asm("_binary_green_fish_png_start");
extern const uint8_t green_fish_png_end[]   asm("_binary_green_fish_png_end");
static volatile SemaphoreHandle_t sendEmailSemaphore = NULL;
//static int write_and_get_response(mbedtls_net_context *sock_fd, unsigned char *buf, size_t len);
//static int write_ssl_and_get_response(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len);
//static int write_ssl_data(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len);
//static int perform_tls_handshake(mbedtls_ssl_context *ssl);
//static void smtp_client_task(void *pvParameters);
void initMailClient();
void sendEmailTask (void *pvParameters);

#endif