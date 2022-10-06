// SplitXex.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// in.xex -1907 -2000 -2800 out.obx
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

typedef struct
{
    int segmentNumber;
    int headerStart;
    int start;
    int size;
    int bufferOffset;
} OneSegment;

int fileSize = 0;
unsigned char* fileSrc = NULL;

OneSegment segments[256] = { 0 };
int numSegments = 0;

void LoadFile(char* filename)
{
    printf("Loading %s...\n", filename);
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("Failed to read the binary input:%s\n", filename);
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    fileSrc = (unsigned char*)malloc(fileSize);
    if (fileSrc == NULL)
    {
        printf("Unable to allocate memory");
        exit(1);
    }
    fread(fileSrc, 1, fileSize, fp);
    fclose(fp);
}

void Analyse(void)
{
    int start;
    int end;
    int size;
    for (int i = 0; i < fileSize;)
    {
        segments[numSegments].segmentNumber = numSegments + 1;
        segments[numSegments].headerStart = i;

        if (fileSrc[i] == 0xFF && fileSrc[i + 1] == 0xFF)
        {
            i += 2;
            segments[numSegments].size += 2;
        }
        start = (fileSrc[i + 1] << 8) + fileSrc[i]; i += 2; segments[numSegments].size += 2;
        end = (fileSrc[i + 1] << 8) + fileSrc[i]; i += 2; segments[numSegments].size += 2;
        size = end - start + 1;

        segments[numSegments].start = start;
        segments[numSegments].size += end - start + 1;
        ++numSegments;

        // Got a block
        printf("Segment #%d : %04X with 0x%04X:%d bytes\n", numSegments, start, size, size);
        i += size;
    }
    printf("\n");
}

int main(int argc, char**argv)
{
    if (argc < 2)
    {
        printf("Split an Atari 8-bit executable file into components and export them.\n");
        printf("Usage:\nSplitXex in.xex [#block #block ... out.obx]\n");
        printf("\tin.xex - the input file. If this is the only parameter list the parts\n");
        printf("\t#1-#255 - The segment number to dump to the output\n");
        printf("\tout.obj - File where data is written to\n");
        exit(1);
    }

    LoadFile(argv[1]);      // Load the input file
    Analyse();
    if (argc == 2)
    {
        printf("\n%d segments found. Specify the segment number to include it in the dump!\n", numSegments);
        printf("i.e. SplitXex %s #1 #2 #3 out.obj\n", argv[1]);
        exit(1);
    }

    // The last command line entry is the target filename, if must not start with a #
    if (argv[argc - 1][0] == '#')
    {
        printf("Missing output file! Last command line argument is:%s\n", argv[argc - 1]);
        exit(1);
    }

    FILE* out = fopen(argv[argc - 1], "wb");
    if (out == NULL)
    {
        printf("Unabled to output data to %s\n", argv[argc - 1]);
        exit(1);
    }
    printf("Writing segments to: %s\n", argv[argc - 1]);
    for (int i = 2; i < argc - 1; ++i)
    {
        char segmentName[256];
        strcpy(segmentName, argv[i]);

        if (segmentName[0] != '#')
        {
            printf("Segment name is not starting with a #, ignoring %s\n", segmentName);
        }
        else 
        {
            // Write the data
            for (int r = strlen(segmentName) - 1; r >= 0; --r)
            {
                if (segmentName[r] == '#') segmentName[r] = ' ';
            }

            int segmentNumber = atoi(segmentName);
            if (segmentNumber > 0 && segmentNumber < 255)
            {
                printf("Copy segment #%d\n", segmentNumber);
                fwrite(fileSrc + segments[segmentNumber - 1].headerStart, 1, segments[segmentNumber - 1].size, out);
            }
            else
            {
                printf("Ignoring segment %s\n", segmentName);
            }
        }

    }



    fclose(out);
    
    printf("Done\n");
}
