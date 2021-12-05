# TO USE:
# pip install Pillow

from PIL import Image, ImageFont, ImageDraw, ImageFilter, ImageOps
import urllib.request
import functools
import io

size = (28, 28)
maxnumber = 9999
count = [0] * 10


@functools.lru_cache
def get_font_from_url(font_url):
    return urllib.request.urlopen(font_url).read()


def webfont(font_url):
    return io.BytesIO(get_font_from_url(font_url))


fonts = [
    "https://github.com/googlefonts/roboto/blob/main/src/hinted/Roboto-Black.ttf?raw=true",  # 103
    "https://github.com/googlefonts/roboto/blob/main/src/hinted/Roboto-Bold.ttf?raw=true",  # 102
    "https://github.com/googlefonts/roboto/blob/main/src/hinted/Roboto-Light.ttf?raw=true",  # 101
    "https://github.com/googlefonts/roboto/blob/main/src/hinted/Roboto-Medium.ttf?raw=true",  # 100
    "https://github.com/googlefonts/roboto/blob/main/src/hinted/Roboto-Regular.ttf?raw=true",  # 99
    "https://github.com/googlefonts/roboto/blob/main/src/hinted/Roboto-Thin.ttf?raw=true",  # 98
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Regular.ttf?raw=true",  # 97
    "https://github.com/googlefonts/nunito/blob/main/fonts/variable/Nunito%5Bwght%5D.ttf?raw=true",  # 96
    "https://github.com/googlefonts/genos/blob/master/fonts/ttf/Genos-Regular.ttf?raw=true",  # 95
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Black.ttf?raw=true",  # 94
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Bold.ttf?raw=true",  # 93
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-ExtraBold.ttf?raw=true",  # 92
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-ExtraLight.ttf?raw=true",  # 91
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Italic.ttf?raw=true",  # 90
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Light.ttf?raw=true",  # 89
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Medium.ttf?raw=true",  # 88
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Regular.ttf?raw=true",  # 87
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-SemiBold.ttf?raw=true",  # 86
    # "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Thin.ttf?raw=true", # 85
    "https://github.com/adobe-fonts/source-sans/blob/release/TTF/SourceSans3-Black.ttf?raw=true",  # 84
    "https://github.com/adobe-fonts/source-sans/blob/release/TTF/SourceSans3-Bold.ttf?raw=true",  # 83
    "https://github.com/adobe-fonts/source-sans/blob/release/TTF/SourceSans3-ExtraLight.ttf?raw=true",  # 82
    "https://github.com/adobe-fonts/source-sans/blob/release/TTF/SourceSans3-Light.ttf?raw=true",  # 81
    "https://github.com/adobe-fonts/source-sans/blob/release/TTF/SourceSans3-Regular.ttf?raw=true",  # 80
    "https://github.com/adobe-fonts/source-sans/blob/release/TTF/SourceSans3-Semibold.ttf?raw=true",  # 79
    "https://github.com/googlefonts/dm-fonts/blob/main/Sans/Exports/DMSans-Bold.ttf?raw=true",  # 78
    "https://github.com/googlefonts/dm-fonts/blob/main/Sans/Exports/DMSans-Medium.ttf?raw=true",  # 77
    "https://github.com/googlefonts/dm-fonts/blob/main/Sans/Exports/DMSans-Regular.ttf?raw=true",  # 76
    "https://github.com/googlefonts/opensans/blob/main/fonts/ttf/OpenSans-Bold.ttf?raw=true",  # 75
    "https://github.com/googlefonts/opensans/blob/main/fonts/ttf/OpenSans-ExtraBold.ttf?raw=true",  # 74
    "https://github.com/googlefonts/opensans/blob/main/fonts/ttf/OpenSans-Light.ttf?raw=true",  # 73
    "https://github.com/googlefonts/opensans/blob/main/fonts/ttf/OpenSans-Regular.ttf?raw=true",  # 72
    "https://github.com/googlefonts/opensans/blob/main/fonts/ttf/OpenSans-SemiBold.ttf?raw=true",  # 71
    "https://github.com/googlefonts/MetrophobicFont/blob/main/fonts/Metrophobic-Regular.ttf?raw=true",  # 70
    "https://github.com/googlefonts/fuzzy-bubbles/blob/master/fonts/ttf/FuzzyBubbles-Bold.ttf?raw=true",  # 69
    "https://github.com/googlefonts/fuzzy-bubbles/blob/master/fonts/ttf/FuzzyBubbles-Regular.ttf?raw=true",  # 68
    "https://github.com/googlefonts/kings/blob/master/fonts/ttf/Kings-Regular.ttf",  # 67
    "https://github.com/googlefonts/Signika/blob/master/fonts/ttf/Signika-Bold.ttf",  # 66
    "https://github.com/googlefonts/Signika/blob/master/fonts/ttf/Signika-Light.ttf",  # 65
    "https://github.com/googlefonts/Signika/blob/master/fonts/ttf/Signika-Medium.ttf",  # 64
    "https://github.com/googlefonts/Signika/blob/master/fonts/ttf/Signika-Regular.ttf",  # 63
    "https://github.com/googlefonts/Signika/blob/master/fonts/ttf/Signika-Semibold.ttf",  # 62
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-Black.ttf",  # 61
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-Bold.ttf",  # 60
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-ExtraBold.ttf",  # 59
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-ExtraLight.ttf",  # 58
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-Light.ttf",  # 57
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-Medium.ttf",  # 56
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-Regular.ttf",  # 55
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-SemiBold.ttf",  # 54
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-Thin.ttf",  # 53
    "https://github.com/googlefonts/ibarrareal/blob/main/fonts/ttf/IbarraRealNova-Bold.ttf",  # 52
    "https://github.com/googlefonts/ibarrareal/blob/main/fonts/ttf/IbarraRealNova-Regular.ttf",  # 51
    "https://github.com/googlefonts/ibarrareal/blob/main/fonts/ttf/IbarraRealNova-SemiBold.ttf",  # 50
    "https://github.com/googlefonts/gideon/blob/master/fonts/ttf/GideonRoman-Regular.ttf",  # 49
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-Black.ttf",  # 48
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-Bold.ttf",  # 47
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-ExtraBold.ttf",  # 46
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-ExtraLight.ttf",  # 45
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-Light.ttf",  # 44
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-Medium.ttf",  # 43
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-Regular.ttf",  # 42
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-SemiBold.ttf",  # 41
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-Thin.ttf",  # 40
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-Black.ttf",  # 39
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-Bold.ttf",  # 38
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-ExtraBold.ttf",  # 37
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-ExtraLight.ttf",  # 36
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-Light.ttf",  # 35
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-Medium.ttf",  # 34
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-Regular.ttf",  # 33
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-SemiBold.ttf",  # 32
    # "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-Thin.ttf",
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-Black.ttf",  # 31
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-Bold.ttf",  # 30
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-ExtraLight.ttf",  # 29
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-Light.ttf",  # 28
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-Medium.ttf",  # 27
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-Regular.ttf",  # 26
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-SemiBold.ttf",  # 25
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-Thin.ttf",  # 24
    # "https://github.com/itfoundry/Poppins/blob/master/variable/TTF%20(Beta)/Poppins-VariableFont_wght.ttf",
    # "https://github.com/clauseggers/Playfair-Display/blob/master/fonts/TTF/PlayfairDisplay-Black.ttf",
    # "https://github.com/clauseggers/Playfair-Display/blob/master/fonts/TTF/PlayfairDisplay-Bold.ttf",
    # "https://github.com/clauseggers/Playfair-Display/blob/master/fonts/TTF/PlayfairDisplay-Regular.ttf",
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-Black.ttf",  # 23
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-Bold.ttf",  # 22
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-ExtraBold.ttf",  # 21
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-Light.ttf",  # 20
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-Medium.ttf",  # 19
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-Regular.ttf",  # 18
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-SemiBold.ttf",  # 17
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-Black.ttf",  # 16
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-Bold.ttf",  # 15
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-ExtraBold.ttf",  # 14
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-ExtraLight.ttf",  # 13
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-Light.ttf",  # 12
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-Medium.ttf",  # 11
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-Regular.ttf",  # 10
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-SemiBold.ttf",  # 9
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-Thin.ttf",  # 8
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-Black.ttf",  # 7
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-Bold.ttf",  # 6
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-ExtraBold.ttf",  # 5
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-Light.ttf",  # 4
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-Medium.ttf",  # 3
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-Regular.ttf",  # 2
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-Thin.ttf",  # 1
]

fonts.reverse()
fontnum = len(fonts)
fontsizes = [(36, -10), (34, -8), (30, -6)]
variationnum = fontnum * len(fontsizes)

for fonturl in fonts:
    for prop in fontsizes:
        (fontsize, ypos) = prop
        with webfont(fonturl + "?raw=true") as f:
            font = ImageFont.truetype(f, size=fontsize)

            for digit in range(0, 10):
                img = Image.new('L', size, 255)

                if digit != 0:
                    draw = ImageDraw.Draw(img)
                    draw.text((3, ypos), str(digit), font=font, fill=0)

                inverted = ImageOps.invert(img)
                inverted.save('../../assets/training_set/' + str(digit) +
                              '-' + format(count[digit], "04") + '.png')
                count[digit] += 1

                print("Generated image batch " + format(count[digit], "04") +
                      "/" + format(variationnum, "04"), end="\r")

                if count[digit] >= maxnumber:
                    break
