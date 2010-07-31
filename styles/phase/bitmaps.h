//////////////////////////////////////////////////////////////////////////////
// bitmaps.h
// -------------------
// Bitmaps for Phase style
// -------------------
// Copyright (c) 2004 David Johnson
// Please see the phasestyle.h file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////

// Note: the "light" bits are drawn with midlight color

// Arrows (6x6)

static TQBitmap uarrow;
static const unsigned char uarrow_bits[] = {
    0x00, 0x0c, 0x1e, 0x3f, 0x3f, 0x00};

static TQBitmap darrow;
static const unsigned char darrow_bits[] = {
    0x00, 0x3f, 0x3f, 0x1e, 0x0c, 0x00};

static TQBitmap larrow;
static const unsigned char larrow_bits[] = {
    0x18, 0x1c, 0x1e, 0x1e, 0x1c, 0x18};

static TQBitmap rarrow;
static const unsigned char rarrow_bits[] = {
    0x06, 0x0e, 0x1e, 0x1e, 0x0e, 0x06};

// plus/minus "arrows"

static TQBitmap bplus;
static const unsigned char bplus_bits[] = {
    0x0c, 0x0c, 0x3f, 0x3f, 0x0c, 0x0c};

static TQBitmap bminus;
static const unsigned char bminus_bits[] = {
    0x00, 0x00, 0x3f, 0x3f, 0x00, 0x00};

// checkmark (9x9)

static TQBitmap bcheck;
static const unsigned char bcheck_bits[] = {
    0x00, 0x00, 0x80, 0x01, 0xc0, 0x01, 0xe0, 0x00, 0x73, 0x00, 0x3f, 0x00,
    0x1e, 0x00, 0x0c, 0x00, 0x00, 0x00};

// expander arrows (9x9)

static TQBitmap dexpand;
static const unsigned char dexpand_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0xfe, 0x00, 0x7c, 0x00, 0x38, 0x00,
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00};

static TQBitmap rexpand;
static const unsigned char rexpand_bits[] = {
    0x04, 0x00, 0x0c, 0x00, 0x1c, 0x00, 0x3c, 0x00, 0x7c, 0x00, 0x3c, 0x00,
    0x1c, 0x00, 0x0c, 0x00, 0x04, 0x00};

// scrollbar doodads (4x4)

static TQBitmap doodad_mid;
static const unsigned char doodad_mid_bits[] = {
    0x07, 0x01, 0x01, 0x00};

static TQBitmap doodad_light;
static const unsigned char doodad_light_bits[] = {
    0x00, 0x08, 0x08, 0x0e};
