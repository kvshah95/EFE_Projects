#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"

static const char *TAG = "store";
static const char *BASE_PATH = "/store";

void write_file(const char *path, const char *content);
void read_file(const char *path);

void fat(void) {
    wl_handle_t wl_handle;
    esp_vfs_fat_mount_config_t mount_config = {
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .max_files = 5,
        .format_if_mount_failed = true,
    };

    // Attempt to mount the FAT filesystem
    esp_err_t ret = esp_vfs_fat_spiflash_mount_rw_wl(BASE_PATH, "storage", &mount_config, &wl_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FAT filesystem. Error: %s", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "FAT filesystem mounted");

    // File operations
    write_file("/store/loaded.txt", "Hello world1");
    read_file("/store/loaded.txt");
    write_file("/store/text.txt", "Hello world!");
    read_file("/store/text.txt");

    // Unmount the FAT filesystem
    esp_vfs_fat_spiflash_unmount_rw_wl(BASE_PATH, wl_handle);
    ESP_LOGI(TAG, "FAT filesystem unmounted");
}

void read_file(const char *path) {
    ESP_LOGI(TAG, "Reading file: %s", path);
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading: %s. errno: %d", strerror(errno), errno);
        return;
    }

    char buffer[100] = {0};
    if (fgets(buffer, sizeof(buffer), file) == NULL && !feof(file)) {
        ESP_LOGE(TAG, "Failed to read from file: %s. errno: %d", strerror(errno), errno);
    } else {
        ESP_LOGI(TAG, "File contains: %s", buffer);
    }
    fclose(file);
}

void write_file(const char *path, const char *content) {
    ESP_LOGI(TAG, "Writing \"%s\" to %s", content, path);
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing: %s. errno: %d", strerror(errno), errno);
        return;
    }

    if (fputs(content, file) == EOF) {
        ESP_LOGE(TAG, "Failed to write to file: %s. errno: %d", strerror(errno), errno);
    }
    fclose(file);
}
