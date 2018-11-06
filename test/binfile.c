#include <stdio.h>

int main(int argc, char **argv)
{
    FILE *fp;

    /*
     * The buffer below is an implementation of loop from 0 to 655334 in the
     * VM's bytecode. This bytecode also stores a static data of 32 bits.
     */
    unsigned char buffer[80] =
    {
        /* Header */
        0xEB, 0x1C, 0xFA, 0x17,
        /* Static Segment Size */
        0x00, 0x00, 0x00, 0x01,
        /* Static Segment Size of 0x0 */
        0x00, 0x00, 0x00, 0x01,
        /* Static Segment 0x0:0x0 */
        0x04, 0x00, 0x00, 0x00, 0x01,
        /* LOAD GPR0 I32 0 */
        0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
        /* LOAD GPR1 I32 1 */
        0x02, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01,
        /* LOAD GPR2 I32 10 */
        0x02, 0x02, 0x04, 0x00, 0x00, 0xFF, 0xFF,
        /* LOAD GPR3 I32 28 */
        0x02, 0x04, 0x04, 0x00, 0x00, 0x00, 0x1C,
        /* ADD GPR0 GPR1 */
        0x15, 0x00, 0x01,
        /* MOVE AR GPR0 */
        0x03, 0x80, 0x00,
        /* LESS_EQ GPR0 GPR2 */
        0x21, 0x00, 0x02,
        /* JUMP_IF_TRUE GPR3 */
        0x13, 0x04,
        /* Halt */
        0x01
    };

    fp = fopen(argv[1], "wb");
    fwrite(buffer, sizeof(buffer), 1, fp);
    fclose(fp);

    return 0;
}
