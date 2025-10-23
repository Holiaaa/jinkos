// simple FAT12 implementation in C
// compile it by doing : gcc fat12.c -o fat12.exe (or fat12)

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#pragma pack(push,1)
typedef struct {
    uint8_t  jmp_boot[3];
    uint8_t  oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t  num_fats;
    uint16_t root_entry_count;
    uint16_t total_sectors_short;
    uint8_t  media;
    uint16_t fat_size_sectors;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_long;
} bpb_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    char name[8];
    char ext[3];
    uint8_t attr; 
    uint8_t nt_reserved;
    uint8_t create_time_tenths;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} dir_entry_t;
#pragma pack(pop)

uint16_t get_fat12_entry(uint8_t *fat, uint16_t cluster) {
    uint32_t offset = (cluster * 3) / 2;
    uint16_t entry;

    if (cluster % 2 == 0) {
        entry = fat[offset] | ((fat[offset + 1] & 0x0F) << 8);
    } else {
        entry = ((fat[offset] & 0xF0) >> 4) | (fat[offset + 1] << 4);
    }

    return entry & 0x0FFF;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: %s <disk image>\n", argv[0]);
        return -1;
    } 

    FILE *disk = fopen(argv[1], "rb");
    if (!disk) {
        perror("can't open the disk image!");
        return -1;
    }

    printf("Disk %s opened successfully!\n", argv[1]);

    bpb_t bpb;
    if (fread(&bpb, sizeof(bpb), 1, disk) != 1) {
        fprintf(stderr, "failed to read BPB of the disk!\n");
        fclose(disk);
        return -1;
    }

    char oem[9];
    memcpy(oem, bpb.oem_name, 8);
    oem[8] = '\0';

    printf("\nOEM :\t\t\t%s", oem);
    printf("\nJumpBoot :\t\t");

    for (int i = 0; i < 3; i++) {
        printf("%02X ", bpb.jmp_boot[i]);
    }

    printf("\nBytesPerSector : \t%u", bpb.bytes_per_sector);
    printf("\nSectorsPerClusters : \t%u", bpb.sectors_per_cluster);
    printf("\nReservedSectorCount : \t%u", bpb.reserved_sector_count);
    printf("\nNumFats : \t\t%u", bpb.num_fats);
    printf("\nRootEntryCount : \t%u", bpb.root_entry_count);
    printf("\nTotalSectorsShort : \t%u", bpb.total_sectors_short);
    printf("\nMedia : \t\t0x%02X", bpb.media);
    printf("\nFatSizeSector : \t%u", bpb.fat_size_sectors);
    printf("\nSectorsPerTrack : \t%u", bpb.sectors_per_track);
    printf("\nNumHeads : \t\t%u", bpb.num_heads);
    printf("\nHiddenSectors : \t%u", bpb.hidden_sectors);
    printf("\nTotalSectorsLong : \t%u", bpb.total_sectors_long);

    uint32_t root_dir_sectors = ((bpb.root_entry_count * 32) + (bpb.bytes_per_sector - 1)) / bpb.bytes_per_sector;
    uint32_t first_root_sector = bpb.reserved_sector_count + (bpb.num_fats * bpb.fat_size_sectors);
    long root_dir_offset = first_root_sector * bpb.bytes_per_sector;

    uint32_t fat_offset = bpb.reserved_sector_count * bpb.bytes_per_sector;
    uint32_t fat_size_bytes = bpb.fat_size_sectors * bpb.bytes_per_sector;
    uint32_t first_data_sector = first_root_sector + root_dir_sectors;
    uint32_t bytes_per_cluster = bpb.bytes_per_sector * bpb.sectors_per_cluster;
    uint32_t root_dir_bytes = root_dir_sectors * bpb.bytes_per_sector;

    uint8_t *fat_buf = malloc(fat_size_bytes);
    if (!fat_buf) {
        fprintf(stderr, "\nfailed to allocate memory for FAT!\n");
        fclose(disk);
        return -1;
    }
    fseek(disk, fat_offset, SEEK_SET);
    if (fread(fat_buf, 1, fat_size_bytes, disk) != fat_size_bytes) {
        fprintf(stderr, "\nfailed to read FAT!\n");
        free(fat_buf);
        fclose(disk);
        return -1;
    }

    // Lire le root directory
    uint8_t *root_buf = malloc(root_dir_bytes);
    if (!root_buf) {
        fprintf(stderr, "\nfailed to allocate memory for root directory!\n");
        free(fat_buf);
        fclose(disk);
        return -1;
    }
    fseek(disk, root_dir_offset, SEEK_SET);
    if (fread(root_buf, 1, root_dir_bytes, disk) != root_dir_bytes) {
        fprintf(stderr, "\nfailed to read root directory from disk!\n");
        free(root_buf);
        free(fat_buf);
        fclose(disk);
        return -1;
    }

    printf("\n\nRoot directory entries:\n");

    for (uint32_t i = 0; i < bpb.root_entry_count; i++) {
        dir_entry_t *de = (dir_entry_t *)(root_buf + i * 32);
        uint8_t first = (uint8_t)de->name[0];

        if (first == 0x00) break;
        if (first == 0xE5) continue;

        if (de->attr & 0x08) {
            char label[9]; memcpy(label, de->name, 8); label[8] = '\0';
            printf("Volume label: %s\n", label);
            continue;
        }

        char name[9], ext[4];
        memcpy(name, de->name, 8); name[8] = '\0';
        memcpy(ext, de->ext, 3); ext[3] = '\0';
        for (int j = 7; j >= 0 && name[j]==' '; j--) name[j]='\0';
        for (int j = 2; j >= 0 && ext[j]==' '; j--) ext[j]='\0';

        if (de->attr & 0x20) {
            if (ext[0]) printf("%s.%s\t", name, ext);
            else printf("%s\t", name);
            printf("[ Attr=0x%02X Size=%uB Cluster=%u ]\n", de->attr, de->file_size, de->first_cluster_low);

            uint16_t cluster = de->first_cluster_low;
            uint32_t remaining = de->file_size;
            uint8_t *file_buf = malloc(remaining);
            if (!file_buf) continue;
            uint32_t buf_pos = 0;

            while (cluster < 0xFF8) {
                long file_offset = (first_data_sector + (cluster - 2) * bpb.sectors_per_cluster) * bpb.bytes_per_sector;
                fseek(disk, file_offset, SEEK_SET);
                uint32_t to_read = (remaining < bytes_per_cluster) ? remaining : bytes_per_cluster;
                fread(file_buf + buf_pos, 1, to_read, disk);
                buf_pos += to_read;
                remaining -= to_read;
                if (remaining == 0) break;
                cluster = get_fat12_entry(fat_buf, cluster);
            }

            printf("Content of %s:\n", name);
            for (uint32_t k = 0; k < de->file_size; k++) printf("%c", file_buf[k]);
            printf("\n\n");
            free(file_buf);

        } else if (de->attr & 0x10) {
            printf("%s <DIR>\t", name);
            printf("[ Attr=0x%02X Size=%uB Cluster=%u ]\n", de->attr, de->file_size, de->first_cluster_low);
        }
    }

    free(root_buf);
    free(fat_buf);
    fclose(disk);
    return 0;
}
