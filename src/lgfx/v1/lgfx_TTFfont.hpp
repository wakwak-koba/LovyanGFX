#ifndef LGFX_TTFFONT_HPP_
#define LGFX_TTFFONT_HPP_

#include "lgfx_fonts.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------
// TTF font

  struct TTFfont : public RunTimeFont
  {
    virtual ~TTFfont();

    font_type_t getType(void) const override { return ft_ttf; }

    std::size_t drawChar(LGFXBase* gfx, std::int32_t x, std::int32_t y, std::uint16_t c, const TextStyle* style) const override;

    void getDefaultMetric(FontMetrics *metrics) const override;

    bool loadFont(DataWrapper* data) override;

    bool unloadFont(void) override;

    bool updateFontMetric(FontMetrics *metrics, std::uint16_t uniCode) const override;

    bool getUnicodeIndex(std::uint16_t unicode, std::uint16_t *index) const;

  private:
    struct TTC_HeaderRec
    {
      std::int32_t version = 0;
      std::uint32_t count = 0;
      std::size_t* offsets = nullptr;
    };
    TTC_HeaderRec _ttc_header;

    struct TT_TableRec
    {
      std::uint32_t Tag;
      std::uint32_t CheckSum;
      std::uint32_t Offset;
      std::uint32_t Length;
    };

    struct SFNT_HeaderRec
    {
      std::uint32_t format_tag;
      std::uint16_t num_tables;
      std::uint16_t search_range;
      std::uint16_t entry_selector;
      std::uint16_t range_shift;
      std::uint32_t offset;  // not in file
    };

    struct TT_Header
    {
      std::int32_t Table_Version;
      std::int32_t Font_Revision;

      std::int32_t CheckSum_Adjust;
      std::int32_t Magic_Number;

      std::uint16_t Flags;
      std::uint16_t Units_Per_EM;

      std::int32_t Created [2];
      std::int32_t Modified[2];

      std::int16_t xMin;
      std::int16_t yMin;
      std::int16_t xMax;
      std::int16_t yMax;

      std::uint16_t Mac_Style;
      std::uint16_t Lowest_Rec_PPEM;

      std::int16_t Font_Direction;
      std::int16_t Index_To_Loc_Format;
      std::int16_t Glyph_Data_Format;
      void load(DataWrapper* data);
    };
    struct TT_MaxProfile
    {
      std::int32_t version;
      std::uint16_t numGlyphs;
      std::uint16_t maxPoints;
      std::uint16_t maxContours;
      std::uint16_t maxCompositePoints;
      std::uint16_t maxCompositeContours;
      std::uint16_t maxZones;
      std::uint16_t maxTwilightPoints;
      std::uint16_t maxStorage;
      std::uint16_t maxFunctionDefs;
      std::uint16_t maxInstructionDefs;
      std::uint16_t maxStackElements;
      std::uint16_t maxSizeOfInstructions;
      std::uint16_t maxComponentElements;
      std::uint16_t maxComponentDepth;
      void load(DataWrapper* data);
    };

    struct TT_HoriHeader
    {
      std::int32_t Version;
      std::int16_t Ascender;
      std::int16_t Descender;
      std::int16_t Line_Gap;
      std::uint16_t advance_Width_Max;      /* advance width maximum */
      std::int16_t min_Left_Side_Bearing;  /* minimum left-sb       */
      std::int16_t min_Right_Side_Bearing; /* minimum right-sb      */
      std::int16_t xMax_Extent;            /* xmax extents          */
      std::int16_t caret_Slope_Rise;
      std::int16_t caret_Slope_Run;
      std::int16_t caret_Offset;
      std::int16_t Reserved[4];
      std::int16_t metric_Data_Format;
      std::uint16_t number_Of_HMetrics;

      void* long_metrics;
      void* short_metrics;
      void load(DataWrapper* data);
    };
    struct TT_VertHeader
    {
      std::int32_t Version;
      std::int16_t Ascender;
      std::int16_t Descender;
      std::int16_t Line_Gap;
      std::uint16_t advance_Height_Max;      /* advance height maximum */
      std::int16_t min_Top_Side_Bearing;    /* minimum left-sb or top-sb       */
      std::int16_t min_Bottom_Side_Bearing; /* minimum right-sb or bottom-sb   */
      std::int16_t yMax_Extent;             /* xmax or ymax extents            */
      std::int16_t caret_Slope_Rise;
      std::int16_t caret_Slope_Run;
      std::int16_t caret_Offset;
      std::int16_t Reserved[4];
      std::int16_t metric_Data_Format;
      std::uint16_t number_Of_VMetrics;

      void* long_metrics;
      void* short_metrics;
      void load(DataWrapper* data);
    };

#define FT_ENC_TAG( value, a, b, c, d )         \
          value = ( ( (std::uint32_t)(a) << 24 ) |  \
                    ( (std::uint32_t)(b) << 16 ) |  \
                    ( (std::uint32_t)(c) <<  8 ) |  \
                      (std::uint32_t)(d)         )

    enum FT_Encoding
    {
      FT_ENC_TAG( FT_ENCODING_NONE, 0, 0, 0, 0 ),

      FT_ENC_TAG( FT_ENCODING_MS_SYMBOL, 's', 'y', 'm', 'b' ),
      FT_ENC_TAG( FT_ENCODING_UNICODE,   'u', 'n', 'i', 'c' ),

      FT_ENC_TAG( FT_ENCODING_SJIS,    's', 'j', 'i', 's' ),
      FT_ENC_TAG( FT_ENCODING_GB2312,  'g', 'b', ' ', ' ' ),
      FT_ENC_TAG( FT_ENCODING_BIG5,    'b', 'i', 'g', '5' ),
      FT_ENC_TAG( FT_ENCODING_WANSUNG, 'w', 'a', 'n', 's' ),
      FT_ENC_TAG( FT_ENCODING_JOHAB,   'j', 'o', 'h', 'a' ),

      /* for backwards compatibility */
      FT_ENCODING_MS_SJIS    = FT_ENCODING_SJIS,
      FT_ENCODING_MS_GB2312  = FT_ENCODING_GB2312,
      FT_ENCODING_MS_BIG5    = FT_ENCODING_BIG5,
      FT_ENCODING_MS_WANSUNG = FT_ENCODING_WANSUNG,
      FT_ENCODING_MS_JOHAB   = FT_ENCODING_JOHAB,

      FT_ENC_TAG( FT_ENCODING_ADOBE_STANDARD, 'A', 'D', 'O', 'B' ),
      FT_ENC_TAG( FT_ENCODING_ADOBE_EXPERT,   'A', 'D', 'B', 'E' ),
      FT_ENC_TAG( FT_ENCODING_ADOBE_CUSTOM,   'A', 'D', 'B', 'C' ),
      FT_ENC_TAG( FT_ENCODING_ADOBE_LATIN_1,  'l', 'a', 't', '1' ),

      FT_ENC_TAG( FT_ENCODING_OLD_LATIN_2, 'l', 'a', 't', '2' ),

      FT_ENC_TAG( FT_ENCODING_APPLE_ROMAN, 'a', 'r', 'm', 'n' )
    };

    struct FT_CharMapRec
    {
//    FT_Face      face;
      FT_Encoding  encoding;
      std::uint16_t platform_id;
      std::uint16_t encoding_id;
    };

    SFNT_HeaderRec _sfnt;
    TT_Header     _header;
    TT_HoriHeader _horizontal;   /* TrueType horizontal header     */
    TT_MaxProfile _max_profile;
    bool          _vertical_info = false;
    TT_VertHeader _vertical;     /* TT Vertical header, if present */

    TT_TableRec* _dir_tables = nullptr;
    std::uint8_t* _cmap_table = nullptr;
    std::uint32_t _cmap_size = 0;

    std::uint32_t horz_metrics_size;
    std::uint32_t horz_metrics_offset;
    std::uint32_t vert_metrics_size;
    std::uint32_t vert_metrics_offset;

    TT_TableRec* tt_face_lookup_table(std::uint32_t tag);
    bool tt_face_goto_table( std::uint32_t tag
                            , DataWrapper* data
                            , std::uint32_t* length = nullptr);

    std::uint_fast8_t face_index = 0;
  };

//----------------------------------------------------------------------------
 }
}
//----------------------------------------------------------------------------

#endif
