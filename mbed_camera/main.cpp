#include "mbed.h"
#include "Camera_LS_Y201.h"
#include "SDFileSystem.h"

//LocalFileSystem local("local");
Camera_LS_Y201 cam1(p28, p27);
SDFileSystem sd(p5, p6, p7, p8, "sd"); 

FILE *fp = NULL;
int datcnt = 0;

/**
 * Callback function for readJpegFileContent.
 *
 * @param buf A pointer to a buffer.
 * @param siz A size of the buffer.
void callback_func(int done, int total, uint8_t *buf, size_t siz) {
    fwrite(buf, siz, 1, fp);

    static int n = 0;
    int tmp = done * 100 / total;
 */
void callback_func(int done, int total, uint8_t *buf, size_t siz) {
    fwrite(buf, siz, 1, fp);

    static int n = 0;
    int tmp = done * 100 / total;
    if (n != tmp) {
        n = tmp;
        // You can print the progress to LCD here.
    }
}

int capture(int n) {
    /*
     * Take a picture.(L)
     */
    if (cam1.takePicture() != 0) {
        return -1;
    }
    printf("Captured.\r\n");

    char fname[64];

    /*
     * Open file.
     * Read the content.
     */
    snprintf(fname, sizeof(fname) - 1, "/sd/IMG_%04d.jpg", n);
    fp = fopen(fname, "wb");
    if (fp == NULL) {
        return -2;
    }
    printf("Capture: %04d", n);
    datcnt = 0;
    if (cam1.readJpegFileContent(callback_func) != 0) {
        fclose(fp);
        return -3;
    }
    fclose(fp);

    /*
     * Stop taking pictures.
     */
    cam1.stopTakingPictures();

    return 0;
}

/**
 * Entry point.
 */
int main(void) {
    wait(1);

    if (cam1.reset() == 0) {
        printf("Reset OK.\r\n");
    }
    wait(1);

    int cnt = 0;
    // for (int i = 0; i < 10; i++) {
    while (1) {
        int r = capture(cnt);
        if (r == 0) {
            printf("[%04d]:OK.\r\n", cnt);
        } else {
            printf("[%04d]:NG. (code=%d)\r\n", cnt, r);
        }
        cnt++;
        wait(45);
    }

//    return 0;
}