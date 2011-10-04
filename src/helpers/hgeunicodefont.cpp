//
#if HGE_UNICODE

#include <hge.h>
#include <hgefont.h>


hgeFont::hgeFont(hgeConstString filename, bool bMipmap)
{
}


void hgeFont::_load_font( hgeConstString filename )
{
    TStrVector lines;
    ba::split (lines, fontdef, ba::is_any_of("\n"));
    TResourcePath font_filename;
    TResourcePath fonts_dir = TResourcePath::Create ("data/fonts");

    foreach (const Str & ln, lines)
    {
        TStrVector words;

        ba::split (words, ln, ba::is_any_of(" "));
        if (words.empty()) continue;

        //--------------------------------------
        // Загрузка параметров из блока info
        //--------------------------------------
        if (words[0] == "info")
        {
            TKeyValuePair size = TKeyValuePair::FromStringArray (words, "size");
            _Size = FromStr <int> (size.Value);
            continue;
        }

        //--------------------------------------
        // Загрузка параметров из блока common
        //--------------------------------------
        if (words[0] == "common")
        {
            TKeyValuePair w = TKeyValuePair::FromStringArray (words, "scaleW");
            int _w = FromStr <int> (w.Value);
            TKeyValuePair h = TKeyValuePair::FromStringArray (words, "scaleH");
            int _h = FromStr <int> (h.Value);
            InvTexW = 1.0f / _w;
            InvTexH = 1.0f / _h;
            continue;
        }

        //--------------------------------------
        // Загрузка текстурной страницы
        //--------------------------------------
        if (words[0] == "page")
        {
            TKeyValuePair   id = TKeyValuePair::FromStringArray (words, "id");
            TKeyValuePair   file = TKeyValuePair::FromStringArray (words, "file");

            // Отрежем в конце файла ".png"
            if (! ba::ends_with (file.Value, ".png"))
            {
                throw XFontLoadingError ("Font texture must be in PNG format");
            }
            file.Value = file.Value.substr (0, file.Value.length()-4);

            if (id.Valid() && file.Valid())
            {
                //CTextureInfo tex_info(EE::TResourcePath::Create("fonts"));
                //tex_info.Filename = file.Value;
                TResourcePath page_path = fonts_dir / file.Value;
                
                int load_texture_result = Textures.LoadTextureAsPage (page_path);
                _ASSERTE (load_texture_result >= 0);

                //Textures.push_back (tex);
            }

            continue;
        }

        //--------------------------------------
        // Загрузка знака шрифта
        //--------------------------------------
        if (words[0] == "char")
        {
            TKeyValuePair   id = TKeyValuePair::FromStringArray (words, "id");
            TKeyValuePair   x = TKeyValuePair::FromStringArray (words, "x");
            TKeyValuePair   y = TKeyValuePair::FromStringArray (words, "y");
            TKeyValuePair   width = TKeyValuePair::FromStringArray (words, "width");
            TKeyValuePair   height = TKeyValuePair::FromStringArray (words, "height");
            TKeyValuePair   xoffset = TKeyValuePair::FromStringArray (words, "xoffset");
            TKeyValuePair   yoffset = TKeyValuePair::FromStringArray (words, "yoffset");
            TKeyValuePair   xadvance = TKeyValuePair::FromStringArray (words, "xadvance");
            TKeyValuePair   page = TKeyValuePair::FromStringArray (words, "page");

            if (id.Valid() && x.Valid() && y.Valid())
            {
                TGlyph      glyph;
                int char_id = FromStr <int> (id.Value);
                glyph.TextureX = FromStr <int> (x.Value);
                glyph.TextureY = FromStr <int> (y.Value);
                glyph.Width = FromStr <int> (width.Value);
                glyph.Height = FromStr <int> (height.Value);
                glyph.XOffset = FromStr <int> (xoffset.Value);
                glyph.YOffset = FromStr <int> (yoffset.Value);
                glyph.XAdvance = FromStr <int> (xadvance.Value);
                glyph.TexturePage = FromStr <int> (page.Value);

                Glyphs [char_id] = glyph;
            }

            continue;
        }

        //--------------------------------------
        // Загрузка кернинга пары символов
        //--------------------------------------
        if (words[0] == "kerning")
        {
            TKeyValuePair   first = TKeyValuePair::FromStringArray (words, "first");
            TKeyValuePair   second = TKeyValuePair::FromStringArray (words, "second");
            TKeyValuePair   amount = TKeyValuePair::FromStringArray (words, "amount");

            if (first.Valid() && second.Valid() && amount.Valid())
            {
                _AddKerningPair (FromStr <int> (first.Value),
                    FromStr <int> (second.Value),
                    FromStr <int> (amount.Value));
            }

            continue;
        }
    }
}

#endif // HGE_UNICODE
