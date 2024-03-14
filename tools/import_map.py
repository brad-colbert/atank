import binascii

PF_PAGE_ROWS = 4
PF_PAGE_COLS = 4

PF_ROW_TILES = 24
PF_COL_TILES = 40

PF_COURSE_ROWS = PF_ROW_TILES * PF_PAGE_ROWS
PF_COURSE_COLS = PF_COL_TILES * PF_PAGE_COLS

#NUM_ROWS_TO_SKIP = int(PF_COURSE_ROWS / (4096 / PF_COURSE_COLS))
NUM_4K_OVERLAPS = int((PF_COURSE_ROWS * PF_COURSE_COLS / 4096) - 1)

print(NUM_4K_OVERLAPS)

with open('speedrunner_grid_test.atrmap.dat', 'rb') as rf:
    row = 1
    addr = 0
    alldata = bytes()

    # Read a row at a time
    data = rf.read(PF_COURSE_COLS)
    while data:
        # Check if this row crosses a 4K (0x1000) boundary.  If so we need to
        # add buffer data to push the next row onto the 4K boundary.
        '''
        next_addr = addr + PF_COURSE_COLS
        over = next_addr % 4096
        skip_bytes = bytes()
        if over < PF_COURSE_COLS:
            skip = (next_addr & 0xFF00) - addr # % 4096
            print('%d:%02x crosses a 4K boundary' %(row, addr))
            next_addr = next_addr & 0xFF00
            print('\tWill skip %d bytes to land on %02X' %(skip, next_addr))
            skip_bytes = bytes(skip)
        '''

        # Write out the bytes
        #print('Writing', len(data), '+', len(skip_bytes))
        alldata = alldata + data #  + skip_bytes

        #addr = next_addr
        data = rf.read(PF_COURSE_COLS)
        row = row + 1

    print('Total rows', row)

    # Pad alldata to end on a PF_COURSE_COLS
    #remaining = PF_COURSE_COLS - (len(alldata) % PF_COURSE_COLS)
    #print('padding', len(alldata), remaining)
    #alldata = alldata + bytes(remaining)

    # Open output file
    with open('playfield_grid_test.h', 'w') as wf:
        wf.write('#ifndef __PLAYFIELD_1_H__\n')
        wf.write('#define __PLAYFIELD_1_H__\n')
        wf.write('#include \"types.h\"\n\n')
        wf.write('const byte playfield[%d][%d] = {\n' % (PF_COURSE_ROWS + NUM_4K_OVERLAPS, PF_COURSE_COLS))

        # Now process loaded data (with skip data)
        col = 0
        row = 0
        for b in alldata:
            if col > 0:
                wf.write(', ')
            else:
                wf.write('{ ')

            wf.write('0x%02X' % b)

            col = col + 1
            if col == PF_COURSE_COLS:
                wf.write('}, // %d\n' % row)
                col = 0

                row = row + 1
                #if row % 24 == 0:
                #    wf.write('')

        wf.write('};\n\n')
        wf.write('#endif//__PLAYFIELD_1_H__\n')

    # Open output file
    import struct
    with open('z2.srm', 'wb') as wf:
        wf.write(struct.pack('B', PF_COURSE_ROWS + NUM_4K_OVERLAPS))
        wf.write(struct.pack('B', PF_COURSE_COLS))

        # Now process loaded data (with skip data)
        col = 0
        row = 0
        for b in alldata:
            wf.write(struct.pack('B', b))
