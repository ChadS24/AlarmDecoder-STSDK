/* ***************************************************************************
 *
 * Copyright 2019-2020 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "st_dev.h"
#include "caps_smokeDetector.h"
#include "esp_log.h"
static const char *TAG = "CAPS_SMOK";

static int caps_smokeDetector_attr_smoke_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_SMOKEDETECTOR_SMOKE_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_smokeDetector.attr_smoke.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_smokeDetector_get_smoke_value(caps_smokeDetector_data_t *caps_data)
{
    if (!caps_data) {
        ESP_LOGE(TAG, "%s: caps_data is NULL", __func__);
        return NULL;
    }
    return caps_data->smoke_value;
}

static void caps_smokeDetector_set_smoke_value(caps_smokeDetector_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        ESP_LOGE(TAG, "%s: caps_data is NULL", __func__);
        return;
    }
    if (caps_data->smoke_value) {
        free(caps_data->smoke_value);
    }
    caps_data->smoke_value = strdup(value);
}

static void caps_smokeDetector_attr_smoke_send(caps_smokeDetector_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        ESP_LOGE(TAG, "%s: fail to get handle", __func__);
        return;
    }
    if (!caps_data->smoke_value) {
        ESP_LOGE(TAG, "%s: value is NULL", __func__);
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_smokeDetector.attr_smoke.name,
            caps_data->smoke_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        ESP_LOGE(TAG, "%s:fail to send smoke value", __func__);
    else
        ESP_LOGI(TAG, "%s: Sequence number return : %d", __func__, sequence_no);

}


static void caps_smokeDetector_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_smokeDetector_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_smokeDetector_attr_smoke_send(caps_data);
}

caps_smokeDetector_data_t *caps_smokeDetector_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_smokeDetector_data_t *caps_data = NULL;

    caps_data = malloc(sizeof(caps_smokeDetector_data_t));
    if (!caps_data) {
        ESP_LOGE(TAG, "%s: fail to malloc for caps_smokeDetector_data", __func__);
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_smokeDetector_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_smoke_value = caps_smokeDetector_get_smoke_value;
    caps_data->set_smoke_value = caps_smokeDetector_set_smoke_value;
    caps_data->attr_smoke_str2idx = caps_smokeDetector_attr_smoke_str2idx;
    caps_data->attr_smoke_send = caps_smokeDetector_attr_smoke_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_smokeDetector.id, caps_smokeDetector_init_cb, caps_data);
    }
    if (!caps_data->handle) {
        ESP_LOGE(TAG, "%s: fail to init smokeDetector handle", __func__);
    }

    return caps_data;
}
