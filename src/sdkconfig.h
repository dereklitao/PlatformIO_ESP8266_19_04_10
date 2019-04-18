/*
 *
 * Automatically generated file; DO NOT EDIT.
 * Espressif IoT Development Framework Configuration
 *
 */
//#define TEST_CASE(...)

#define CONFIG_ESP_WIFI_SSID "esp-wifi"
#define CONFIG_ESP_WIFI_PASSWORD "wifi"
#define CONFIG_MAX_STA_CONN 4

//#define CONFIG_ENABLE_PTHREAD 1
#define CONFIG_MQTT_USING_ESP 1
#define CONFIG_MQTT_PROTOCOL_311 1
#define CONFIG_USE_ONLY_LWIP_SELECT 1
#define MQTT_BUFFER_SIZE_BYTE 4096
#define CONFIG_MQTT_TASK_STACK_SIZE 10240
#define CONFIG_ESP8266_PHY_MAX_WIFI_TX_POWER 20
#define CONFIG_FREERTOS_IDLE_TASK_STACKSIZE 2048

#define CONFIG_ENABLE_MDNS 1
#define CONFIG_MDNS_MAX_SERVICES 10
#define CONFIG_MDNS_STACKSIZE 2048
#define CONFIG_LWIP_IPV6 1
#define CONFIG_LWIP_IPV6_NUM_ADDRESSES 3
#define CONFIG_AWS_IOT_SDK 1
#define CONFIG_AWS_IOT_MQTT_HOST "a3l8rpjg868svp.iot.eu-central-1.amazonaws.com"
#define CONFIG_AWS_IOT_MQTT_PORT 8883
#define CONFIG_AWS_IOT_MQTT_TX_BUF_LEN 512
#define CONFIG_AWS_IOT_MQTT_RX_BUF_LEN 512
#define CONFIG_AWS_IOT_MQTT_NUM_SUBSCRIBE_HANDLERS 5
#define CONFIG_AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL 1000
#define CONFIG_AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL 128000
#define CONFIG_AWS_IOT_SHADOW_MAX_SIZE_OF_THING_NAME 20
#define CONFIG_AWS_IOT_SHADOW_MAX_JSON_TOKEN_EXPECTED 120
#define CONFIG_AWS_IOT_SHADOW_MAX_SIMULTANEOUS_ACKS 10
#define CONFIG_AWS_IOT_SHADOW_MAX_SIMULTANEOUS_THINGNAMES 10
#define CONFIG_AWS_IOT_SHADOW_MAX_SHADOW_TOPIC_LENGTH_WITHOUT_THINGNAME 60
#define CONFIG_HTTPD_MAX_REQ_HDR_LEN 512
#define CONFIG_HTTPD_MAX_URI_LEN 512
#define CONFIG_ENABLE_BOOT_CHECK_SUM 1
#define CONFIG_IDF_TARGET_ESP8266 1
#define CONFIG_MBEDTLS_XTEA_C 1
#define CONFIG_TCP_RECVMBOX_SIZE 6
#define CONFIG_TCP_WND_DEFAULT 5840
#define CONFIG_PARTITION_TABLE_OFFSET 0x8000
#define CONFIG_TCPIP_ADAPTER_GLOBAL_DATA_LINK_IRAM 1
#define CONFIG_ESPTOOLPY_FLASHSIZE_4MB 1
#define CONFIG_FREERTOS_GLOBAL_DATA_LINK_IRAM 1
#define CONFIG_MQTT_RECV_BUFFER 2048
#define CONFIG_ESPTOOLPY_FLASHFREQ "40m"
#define CONFIG_MBEDTLS_KEY_EXCHANGE_RSA 1
#define CONFIG_CRYSTAL_USED_26MHZ 1
#define CONFIG_UDP_RECVMBOX_SIZE 6
#define CONFIG_MBEDTLS_AES_C 1
#define CONFIG_MBEDTLS_RSA_BITLEN_MIN 2048
#define CONFIG_ESPTOOLPY_FLASHSIZE "4MB"
#define CONFIG_LOG_DEFAULT_LEVEL_VERBOSE 0
#define CONFIG_LWIP_ICMP 1
#define CONFIG_MBEDTLS_SSL_PROTO_TLS1 1
#define CONFIG_SPI_FLASH_MODE 0x0
#define CONFIG_ESPTOOLPY_FLASHFREQ_40M 1
#define CONFIG_ESP_WIFI_IS_STATION 1
#define CONFIG_DEFAULT_MQTT_SECURITY 0
#define CONFIG_MBEDTLS_PEM_WRITE_C 1
#define CONFIG_MBEDTLS_SSL_OUT_CONTENT_LEN 4096
#define CONFIG_IP_LOST_TIMER_INTERVAL 120
#define CONFIG_APP1_SIZE 0xF0000
#define CONFIG_LWIP_GLOBAL_DATA_LINK_IRAM 1
#define CONFIG_CONSOLE_UART_BAUDRATE 74880
#define CONFIG_LWIP_MAX_SOCKETS 10
#define CONFIG_BOOTLOADER_CHECK_APP_SUM 1
#define CONFIG_SSL_USING_MBEDTLS 1
#define CONFIG_MONITOR_BAUD_74880B 1
#define CONFIG_NEWLIB_LIBRARY_LEVEL_NANO 1
#define CONFIG_TCP_MSS 1460
#define CONFIG_LWIP_RECV_BUFSIZE_DEFAULT 11680
#define CONFIG_ESP_UDP_SYNC_RETRY_MAX 5
#define CONFIG_ESP8266_CORE_GLOBAL_DATA_LINK_IRAM 1
#define CONFIG_LWIP_MAX_UDP_PCBS 4
#define CONFIG_ESPTOOLPY_BAUD 921600
#define CONFIG_ESPTOOLPY_AFTER_RESET 1
#define CONFIG_OPTIMIZATION_ASSERTIONS_ENABLED 1
#define CONFIG_LWIP_DHCPS_MAX_STATION_NUM 8
#define CONFIG_TOOLPREFIX "xtensa-lx106-elf-"
#define CONFIG_MBEDTLS_RC4_DISABLED 1
#define CONFIG_CONSOLE_UART_NUM 0
#define CONFIG_APP1_OFFSET 0x10000
#define CONFIG_MQTT_USERNAME "espressif"
#define CONFIG_TCP_OVERSIZE_MSS 1
#define CONFIG_CONSOLE_UART_DEFAULT 1
#define CONFIG_MBEDTLS_X509_CRL_PARSE_C 1
#define CONFIG_SPI_FLASH_SIZE 0x400000
#define CONFIG_LWIP_DHCPS_LEASE_UNIT 60
#define CONFIG_TCPIP_TASK_STACK_SIZE 4096
#define CONFIG_TASK_WDT 1
#define CONFIG_MAIN_TASK_STACK_SIZE 3584
#define CONFIG_LWIP_MAX_ACTIVE_TCP 5
#define CONFIG_TASK_WDT_TIMEOUT_S 15
#define CONFIG_ESPTOOLPY_FLASHMODE "dio"
#define CONFIG_MBEDTLS_DHM_C 1
#define CONFIG_LWIP_ARP_TABLE_SIZE 10
#define CONFIG_ESPTOOLPY_BEFORE "default_reset"
#define CONFIG_LOG_DEFAULT_LEVEL 2
#define CONFIG_MAKE_WARN_UNDEFINED_VARIABLES 1
#define CONFIG_NO_TLS 1
#define CONFIG_V3_1 1
#define CONFIG_MAX_STA_CONN 4
#define CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE 1
#define CONFIG_BOOTLOADER_INIT_SPI_FLASH 1
#define CONFIG_LIBSODIUM_USE_MBEDTLS_SHA 1
#define CONFIG_TCP_SYNMAXRTX 6
#define CONFIG_LWIP_IGMP 1
#define CONFIG_PYTHON "python"
#define CONFIG_ESPTOOLPY_COMPRESSED 1
#define CONFIG_MBEDTLS_RSA_BITLEN_2048 1
#define CONFIG_PARTITION_TABLE_FILENAME "partitions_singleapp.csv"
#define CONFIG_TCP_SND_BUF_DEFAULT 2920
#define CONFIG_LWIP_DHCP_MAX_NTP_SERVERS 1
#define CONFIG_FREERTOS_TIMER_STACKSIZE 2048
#define CONFIG_MBEDTLS_SSL_PROTO_TLS1_1 1
#define CONFIG_LWIP_SO_REUSE_RXTOALL 1
#define CONFIG_PARTITION_TABLE_SINGLE_APP 1
#define CONFIG_MBEDTLS_X509_CSR_PARSE_C 1
#define CONFIG_CLEAN_SESSION 1
#define CONFIG_DEFAULT_MQTT_VERSION 3
#define CONFIG_USING_NEW_ETS_VPRINTF 1
#define CONFIG_FLASHMODE_QIO 1
#define CONFIG_MQTT_KEEP_ALIVE 30
#define CONFIG_LWIP_DHCP_DOES_ARP_CHECK 1
#define CONFIG_LOG_BOOTLOADER_LEVEL_VERBOSE 0
#define CONFIG_ESPTOOLPY_ENABLE_TIME 1
#define CONFIG_TASK_WDT_TIMEOUT_15N 1
#define CONFIG_MBEDTLS_PEM_PARSE_C 1
#define CONFIG_LWIP_SOCKET_MULTITHREAD 1
#define CONFIG_TASK_SWITCH_FASTER 1
#define CONFIG_MBEDTLS_SSL_PROTO_TLS1_2 1
#define CONFIG_LWIP_TCP_CLOSE_TIMEOUT_MS_DEFAULT 10000
#define CONFIG_APP_UPDATE_CHECK_APP_SUM 1
#define CONFIG_FREERTOS_HZ 100
//#define CONFIG_LOG_COLORS 1
#define CONFIG_STACK_CHECK_NONE 1
#define CONFIG_LOG_BOOTLOADER_LEVEL 2
#define CONFIG_MBEDTLS_TLS_ENABLED 1
#define CONFIG_LWIP_MAX_RAW_PCBS 4
#define CONFIG_ESPTOOLPY_BEFORE_RESET 1
#define CONFIG_MQTT_SEND_CYCLE 30000
#define CONFIG_ESP_FILENAME_MACRO_NO_PATH 1
#define CONFIG_ESPTOOLPY_BAUD_OTHER_VAL 115200
#define CONFIG_MQTT_SEND_BUFFER 2048
#define CONFIG_SCAN_AP_MAX 32
#define CONFIG_ESP_UDP_SYNC_SEND 1
#define CONFIG_ESP32_PTHREAD_TASK_PRIO_DEFAULT 5
#define CONFIG_TCPIP_RECVMBOX_SIZE 32
#define CONFIG_TCP_MAXRTX 12
#define CONFIG_ESPTOOLPY_AFTER "hard_reset"
#define CONFIG_LWIP_SO_REUSE 1
#define CONFIG_LWIP_SO_REUSE 1
#define CONFIG_WIFI_PPT_TASKSTACK_SIZE 2048
#define CONFIG_LWIP_MAX_LISTENING_TCP 8
#define CONFIG_MBEDTLS_TLS_CLIENT 1
#define CONFIG_MONITOR_BAUD 74880
#define CONFIG_PARTITION_TABLE_CUSTOM_FILENAME "partitions_singleapp.csv"
#define CONFIG_DEFAULT_MQTT_SESSION 1
#define CONFIG_MBEDTLS_HAVE_TIME 1
#define CONFIG_MBEDTLS_TLS_SERVER 1
#define CONFIG_MBEDTLS_TLS_SERVER_AND_CLIENT 1
#define CONFIG_MQTT_CLIENT_ID "espressif_sample"
#define CONFIG_FREERTOS_ISR_STACKSIZE 512
#define CONFIG_LWIP_IP_REASS_MAX_PBUFS 10
#define CONFIG_NEWLIB_ENABLE 1
#define CONFIG_OPENSSL_ASSERT_DO_NOTHING 1
#define CONFIG_SPI_FLASH_FREQ 0x0
#define CONFIG_OPTIMIZATION_LEVEL_DEBUG 1
#define CONFIG_DNS_MAX_SERVERS 2
#define CONFIG_LWIP_ARP_MAXAGE 300
#define CONFIG_LTM_FAST 1
#define CONFIG_MQTT_PING_TIMEOUT 3000
#define CONFIG_ESPTOOLPY_BAUD_921600B 1
#define CONFIG_MQTT_RECV_CYCLE 0
#define CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN 4096
#define CONFIG_MQTT_PASSWORD "admin"
#define CONFIG_RESET_REASON 1
#define CONFIG_FREERTOS_MAX_HOOK 2
#define CONFIG_SOC_IRAM_SIZE 0xC000
#define CONFIG_WIFI_TX_RATE_SEQUENCE_FROM_HIGH 1
#define CONFIG_SET_SOLINGER_DEFAULT 1
#define CONFIG_ESP32_PTHREAD_TASK_STACK_SIZE_DEFAULT 3072
#define CONFIG_MONITOR_BAUD_OTHER_VAL 74880
#define CONFIG_NEWLIB_STDOUT_LINE_ENDING_CRLF 1
#define CONFIG_ESPTOOLPY_PORT "/dev/ttyUSB0"
#define CONFIG_TASK_WDT_PANIC 1
#define CONFIG_EVENT_LOOP_STACK_SIZE 2048
#define CONFIG_ESP_HTTP_CLIENT_ENABLE_HTTPS 1