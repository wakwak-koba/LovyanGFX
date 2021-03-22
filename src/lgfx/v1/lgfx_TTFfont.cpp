#include "lgfx_TTFfont.hpp"

#include "platforms/common.hpp"
#include "misc/pixelcopy.hpp"
#include "LGFXBase.hpp"


//#include <esp_log.h> // for log output
//#include <Arduino.h> // for log output

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  TTFfont::~TTFfont()
  {
    unloadFont();
  }

  std::size_t TTFfont::drawChar(LGFXBase* gfx, std::int32_t x, std::int32_t y, std::uint16_t c, const TextStyle* style) const
  {
    return 0;
  }

  void TTFfont::getDefaultMetric(FontMetrics *metrics) const
  {
    
  }

  bool TTFfont::loadFont(DataWrapper* data)
  {
//Serial.println("TTFfont loadFont");
    #define FT_MAKE_TAG( _x1, _x2, _x3, _x4 ) \
              ( ( (std::uint32_t)_x1 << 24 ) |     \
                ( (std::uint32_t)_x2 << 16 ) |     \
                ( (std::uint32_t)_x3 <<  8 ) |     \
                  (std::uint32_t)_x4         )

    static constexpr std::uint32_t TTAG_ttcf = FT_MAKE_TAG( 't', 't', 'c', 'f' );
    static constexpr std::uint32_t TTAG_true = FT_MAKE_TAG( 't', 'r', 'u', 'e' );
//    static constexpr std::uint32_t TTAG_OTTO = FT_MAKE_TAG( 'O', 'T', 'T', 'O' );
//    static constexpr std::uint32_t TTAG_typ1 = FT_MAKE_TAG( 't', 'y', 'p', '1' );

    static constexpr std::uint32_t TTAG_glyf = FT_MAKE_TAG( 'g', 'l', 'y', 'f' );
    static constexpr std::uint32_t TTAG_CFF  = FT_MAKE_TAG( 'C', 'F', 'F', ' ' );

    static constexpr std::uint32_t TTAG_head = FT_MAKE_TAG( 'h', 'e', 'a', 'd' );
    static constexpr std::uint32_t TTAG_maxp = FT_MAKE_TAG( 'm', 'a', 'x', 'p' );
    static constexpr std::uint32_t TTAG_cmap = FT_MAKE_TAG( 'c', 'm', 'a', 'p' );
//    static constexpr std::uint32_t TTAG_name = FT_MAKE_TAG( 'n', 'a', 'm', 'e' );
    static constexpr std::uint32_t TTAG_hhea = FT_MAKE_TAG( 'h', 'h', 'e', 'a' );
    static constexpr std::uint32_t TTAG_hmtx = FT_MAKE_TAG( 'h', 'm', 't', 'x' );
    static constexpr std::uint32_t TTAG_vhea = FT_MAKE_TAG( 'v', 'h', 'e', 'a' );
    static constexpr std::uint32_t TTAG_vmtx = FT_MAKE_TAG( 'v', 'm', 't', 'x' );

    unloadFont();

    _fontData = data;

    std::uint32_t tag = data->read32swap();

    if (tag != 0x00010000UL
     && tag != 0x00020000UL
     && tag != TTAG_true
     && tag != TTAG_ttcf
//     && tag != TTAG_OTTO
//     && tag != TTAG_typ1 
    )
    {
      return false;
    }

    if ( tag == TTAG_ttcf )
    {
      _ttc_header.version = data->read32swap();
      _ttc_header.count   = data->read32swap();
      if (_ttc_header.count <= 0)
      {
        return false;
      }
      _ttc_header.offsets = (std::size_t*)lgfx::heap_alloc(4 * _ttc_header.count);
      for (std::uint32_t i = 0; i < _ttc_header.count; i++)
      {
        _ttc_header.offsets[i] = data->read32swap();
      }
    }
    else
    {
      _ttc_header.version = 1 << 16;
      _ttc_header.count   = 1;
      _ttc_header.offsets = (std::size_t*)lgfx::heap_alloc(4);
      _ttc_header.offsets[0] = 0;
    }

//-----

    if (_ttc_header.count >= face_index) face_index = 0;

    _sfnt.offset = _ttc_header.offsets[face_index];

    if ( ! data->seek( _sfnt.offset ) )
    {
      return false;
    }
    _sfnt.format_tag     = data->read32swap();
    _sfnt.num_tables     = data->read16swap();
    _sfnt.search_range   = data->read16swap();
    _sfnt.entry_selector = data->read16swap();
    _sfnt.range_shift    = data->read16swap();

    if (_sfnt.format_tag != 0x00010000UL
     && _sfnt.format_tag != 0x00020000UL
     && _sfnt.format_tag != TTAG_true)
    {
      return false;
    }

//ESP_LOGI("lgfx_fonts", "-- Number of tables: %10u ",    _sfnt.num_tables );
//ESP_LOGI("lgfx_fonts", "-- Format version:   0x%08lx", _sfnt.format_tag );
    // if ( _sfnt.format_tag != TTAG_OTTO ) { check_table_dir }
    if (_sfnt.num_tables <= 0)
    {
      return false;
    }

    _dir_tables = (TT_TableRec*)lgfx::heap_alloc(sizeof(TT_TableRec) * _sfnt.num_tables);
    if ( _dir_tables == nullptr )
    {
      return false;
    }

    auto entry = _dir_tables;

    int i = 0;
    do
    {
      entry[i].Tag      = data->read32swap();
      entry[i].CheckSum = data->read32swap();
      entry[i].Offset   = data->read32swap();
      entry[i].Length   = data->read32swap();
/*
      ESP_LOGI("lgfx_fonts"
              , "  %c%c%c%c  %08lx  %08lx  %08lx",
                    (char)( entry[i].Tag >> 24 ),
                    (char)( entry[i].Tag >> 16 ),
                    (char)( entry[i].Tag >> 8  ),
                    (char)( entry[i].Tag       ),
                    entry[i].Offset,
                    entry[i].Length,
                    entry[i].CheckSum );
//*/
    } while (++i < _sfnt.num_tables);

// ---- sfnt_load_face
    // check has outline
    if ( tt_face_lookup_table( TTAG_glyf ) == nullptr
     &&  tt_face_lookup_table( TTAG_CFF )  == nullptr )
    {
//ESP_LOGI("lgfx_fonts","not outline font");
      return false;
    }

    if (!tt_face_goto_table( TTAG_head, data )) { return false; }
    _header.load(data);

//ESP_LOGI("lgfx_fonts", "Units per EM: %4u", _header.Units_Per_EM );
//ESP_LOGI("lgfx_fonts", "IndexToLoc:   %4d", _header.Index_To_Loc_Format );
    if (!tt_face_goto_table( TTAG_maxp, data )) { return false; }
    _max_profile.load(data);

    if (!tt_face_goto_table( TTAG_cmap, data, &_cmap_size )) { return false; }
    _cmap_table = (std::uint8_t*)lgfx::heap_alloc_psram(_cmap_size);
    if (_cmap_table == nullptr)
    {
      _cmap_table = (std::uint8_t*)lgfx::heap_alloc(_cmap_size);
    }
//ESP_LOGI("lgfx_fonts", "cmap_size: %u", _cmap_size );
//ESP_LOGI("lgfx_fonts", "cmap_table: %04x", (int)_cmap_table );
    if (_cmap_table == nullptr) { return false; }

    data->read(_cmap_table, _cmap_size);

//  if (!tt_face_goto_table( TTAG_name, data )) { return false; }

// ---- tt_face_load_hhea
    if (!tt_face_goto_table( TTAG_hhea, data )) { return false; }
    _horizontal.load(data);

    if (!tt_face_goto_table( TTAG_hmtx, data, &horz_metrics_size)) { return false; }
    horz_metrics_offset = data->tell();

    _vertical_info = false;
    if (tt_face_goto_table( TTAG_vhea, data ))
    {
      _vertical.load(data);
      if (tt_face_goto_table( TTAG_vmtx, data, &vert_metrics_size))
      {
        vert_metrics_offset = data->tell();
        _vertical_info = true;
      }
    }

// ---- find_unicode_charmap


    return true;
  }

  bool TTFfont::unloadFont(void)
  {
    if (_ttc_header.offsets != nullptr) { heap_free(_ttc_header.offsets); _ttc_header.offsets = nullptr; }
    if (_dir_tables != nullptr) { heap_free(_dir_tables);  _dir_tables = nullptr; }
    if (_cmap_table != nullptr) { heap_free(_cmap_table);  _cmap_table = nullptr; }

    _fontLoaded = false;
    _cmap_size = 0;

    return true;
  }

  bool TTFfont::updateFontMetric(FontMetrics *metrics, std::uint16_t uniCode) const
  {
    return false;
  }

  bool TTFfont::getUnicodeIndex(std::uint16_t unicode, std::uint16_t *index) const
  {
    return false;
  }

  TTFfont::TT_TableRec* TTFfont::tt_face_lookup_table(std::uint32_t tag)
  {
    TT_TableRec* entry = _dir_tables;
    TT_TableRec* limit = entry + _sfnt.num_tables;
    for ( ; entry < limit; entry++ )
    {
      if ( entry->Tag != tag ) continue;
      if ( entry->Length == 0 ) continue;
//    ESP_LOGI(( "found table.\n" ));
      return entry;
    }
    return nullptr;
  }

  bool TTFfont::tt_face_goto_table( std::uint32_t tag
                                  , DataWrapper* data
                                  , std::uint32_t* length )
  {
    auto table = tt_face_lookup_table( tag );
    if ( table )
    {
      if ( length )
        *length = table->Length;

      if ( data->seek( table->Offset ) )
        return true;
    }
    return false;
  }

  void TTFfont::TT_Header::load(DataWrapper* data)
  {
    Table_Version      = data->read32swap();
    Font_Revision      = data->read32swap();
    CheckSum_Adjust    = data->read32swap();
    Magic_Number       = data->read32swap();
    Flags              = data->read16swap();
    Units_Per_EM       = data->read16swap();
    Created[0]         = data->read32swap();
    Created[1]         = data->read32swap();
    Modified[0]        = data->read32swap();
    Modified[1]        = data->read32swap();
    xMin               = data->read16swap();
    yMin               = data->read16swap();
    xMax               = data->read16swap();
    yMax               = data->read16swap();
    Mac_Style          = data->read16swap();
    Lowest_Rec_PPEM    = data->read16swap();
    Font_Direction     = data->read16swap();
    Index_To_Loc_Format= data->read16swap();
    Glyph_Data_Format  = data->read16swap();
  }

  void TTFfont::TT_MaxProfile::load(DataWrapper* data)
  {
    memset(this, 0, sizeof(TTFfont::TT_MaxProfile));
    version              = data->read32swap();
    numGlyphs            = data->read16swap();
//ESP_LOGI("lgfx_fonts", "numGlyphs: %u", numGlyphs );
    if ( version >= 0x10000L )
    {
      maxPoints            = data->read16swap();
      maxContours          = data->read16swap();
      maxCompositePoints   = data->read16swap();
      maxCompositeContours = data->read16swap();
      maxZones             = data->read16swap();
      maxTwilightPoints    = std::min<std::uint16_t>( 0xFFFFu - 4, data->read16swap());
      maxStorage           = data->read16swap();
      maxFunctionDefs      = std::max<std::uint16_t>(64u, data->read16swap());
      maxInstructionDefs   = data->read16swap();
      maxStackElements     = data->read16swap();
      maxSizeOfInstructions= data->read16swap();
      maxComponentElements = data->read16swap();
      maxComponentDepth    = std::max<std::uint16_t>(100, data->read16swap());
    }
  }

  void TTFfont::TT_HoriHeader::load(DataWrapper* data)
  {
    Version                = data->read32swap();
    Ascender               = data->read16swap();
    Descender              = data->read16swap();
    Line_Gap               = data->read16swap();
    advance_Width_Max      = data->read16swap();
    min_Left_Side_Bearing  = data->read16swap();
    min_Right_Side_Bearing = data->read16swap();
    xMax_Extent            = data->read16swap();
    caret_Slope_Rise       = data->read16swap();
    caret_Slope_Run        = data->read16swap();
    caret_Offset           = data->read16swap();
    Reserved[0]            = data->read16swap();
    Reserved[1]            = data->read16swap();
    Reserved[2]            = data->read16swap();
    Reserved[3]            = data->read16swap();
    metric_Data_Format     = data->read16swap();
    number_Of_HMetrics     = data->read16swap();

// ESP_LOGI("lgfx_fonts", "hori Ascender:          %5d", Ascender );
// ESP_LOGI("lgfx_fonts", "hori Descender:         %5d", Descender );
// ESP_LOGI("lgfx_fonts", "hori number_Of_Metrics: %5u", number_Of_HMetrics );

    long_metrics  = NULL;
    short_metrics = NULL;
  }

  void TTFfont::TT_VertHeader::load(DataWrapper* data)
  {
    Version                = data->read32swap();
    Ascender               = data->read16swap();
    Descender              = data->read16swap();
    Line_Gap               = data->read16swap();
    advance_Height_Max     = data->read16swap();
    min_Top_Side_Bearing   = data->read16swap();
    min_Bottom_Side_Bearing= data->read16swap();
    yMax_Extent            = data->read16swap();
    caret_Slope_Rise       = data->read16swap();
    caret_Slope_Run        = data->read16swap();
    caret_Offset           = data->read16swap();
    Reserved[0]            = data->read16swap();
    Reserved[1]            = data->read16swap();
    Reserved[2]            = data->read16swap();
    Reserved[3]            = data->read16swap();
    metric_Data_Format     = data->read16swap();
    number_Of_VMetrics     = data->read16swap();

// ESP_LOGI("lgfx_fonts", "vert Ascender:          %5d", Ascender );
// ESP_LOGI("lgfx_fonts", "vert Descender:         %5d", Descender );
// ESP_LOGI("lgfx_fonts", "vert number_Of_Metrics: %5u", number_Of_VMetrics );

    long_metrics  = NULL;
    short_metrics = NULL;
  }

//----------------------------------------------------------------------------
 }
}
