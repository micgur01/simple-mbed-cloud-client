// ----------------------------------------------------------------------------
// Copyright 2016-2018 ARM Ltd.
//
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------------------------------------------------------

#include "update_ui_example.h"

#ifdef MBED_CLOUD_CLIENT_SUPPORT_UPDATE

#include <stdio.h>
#include <stdint.h>

static MbedCloudClient* _client;

#ifdef ARM_UPDATE_CLIENT_VERSION_VALUE
#if ARM_UPDATE_CLIENT_VERSION_VALUE > 101000
void update_ui_set_cloud_client(MbedCloudClient* client)
{
    _client = client;
}

void __attribute__((weak)) update_authorize(int32_t request)
{
    switch (request)
    {
        /* Cloud Client wishes to download new firmware. This can have a negative
           impact on the performance of the rest of the system.

           The user application is supposed to pause performance sensitive tasks
           before authorizing the download.

           Note: the authorization call can be postponed and called later.
           This doesn't affect the performance of the Cloud Client.
        */
        case MbedCloudClient::UpdateRequestDownload:
            printf("Firmware download requested\r\n");
            printf("Authorization granted\r\n");
            _client->update_authorize(MbedCloudClient::UpdateRequestDownload);
            break;

        /* Cloud Client wishes to reboot and apply the new firmware.

           The user application is supposed to save all current work before rebooting.

           Note: the authorization call can be postponed and called later.
           This doesn't affect the performance of the Cloud Client.
        */
        case MbedCloudClient::UpdateRequestInstall:
            printf("Firmware install requested\r\n");
            printf("Authorization granted\r\n");
            _client->update_authorize(MbedCloudClient::UpdateRequestInstall);
            break;

        default:
            printf("Error - unknown request\r\n");
            break;
    }
}
#endif
#endif

void __attribute__((weak)) update_progress(uint32_t progress, uint32_t total)
{
    uint8_t percent = progress * 100 / total;

/* only show progress bar if debug trace is disabled */
#if (!defined(MBED_CONF_MBED_TRACE_ENABLE) || MBED_CONF_MBED_TRACE_ENABLE == 0) \
    && !ARM_UC_ALL_TRACE_ENABLE \
    && !ARM_UC_HUB_TRACE_ENABLE

    printf("\rDownloading: [");
    for (uint8_t index = 0; index < 50; index++)
    {
        if (index < percent / 2)
        {
            printf("+");
        }
        else if (index == percent / 2)
        {
            static uint8_t old_max = 0;
            static uint8_t counter = 0;

            if (index == old_max)
            {
                counter++;
            }
            else
            {
                old_max = index;
                counter = 0;
            }

            switch (counter % 4)
            {
                case 0:
                    printf("/");
                    break;
                case 1:
                    printf("-");
                    break;
                case 2:
                    printf("\\");
                    break;
                case 3:
                default:
                    printf("|");
                    break;
            }
        }
        else
        {
            printf(" ");
        }
    }
    printf("] %d %%", percent);
    fflush(stdout);
#else
    printf("Downloading: %d %%\r\n", percent);
#endif

    if (progress == total)
    {
        printf("\r\nDownload completed\r\n");
    }
}

#endif // MBED_CLOUD_CLIENT_SUPPORT_UPDATE
