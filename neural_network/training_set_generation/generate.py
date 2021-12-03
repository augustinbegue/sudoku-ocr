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
    "https://github.com/googlefonts/roboto/blob/main/src/hinted/Roboto-Black.ttf?raw=true",
    "https://github.com/googlefonts/roboto/blob/main/src/hinted/Roboto-Bold.ttf?raw=true",
    "https://github.com/googlefonts/roboto/blob/main/src/hinted/Roboto-Light.ttf?raw=true",
    "https://github.com/googlefonts/roboto/blob/main/src/hinted/Roboto-Medium.ttf?raw=true",
    "https://github.com/googlefonts/roboto/blob/main/src/hinted/Roboto-Regular.ttf?raw=true",
    "https://github.com/googlefonts/roboto/blob/main/src/hinted/Roboto-Thin.ttf?raw=true",
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Regular.ttf?raw=true",
    "https://github.com/googlefonts/nunito/blob/main/fonts/variable/Nunito%5Bwght%5D.ttf?raw=true",
    # "https://github.com/googlefonts/smooch/blob/master/fonts/ttf/Smooch-Regular.ttf?raw=true",
    "https://github.com/googlefonts/genos/blob/master/fonts/ttf/Genos-Regular.ttf?raw=true",
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Black.ttf?raw=true",
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Bold.ttf?raw=true",
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-ExtraBold.ttf?raw=true",
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-ExtraLight.ttf?raw=true",
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Italic.ttf?raw=true",
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Light.ttf?raw=true",
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Medium.ttf?raw=true",
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Regular.ttf?raw=true",
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-SemiBold.ttf?raw=true",
    "https://github.com/googlefonts/noto-fonts/blob/main/hinted/ttf/NotoSans/NotoSans-Thin.ttf?raw=true",
    "https://github.com/adobe-fonts/source-sans/blob/release/TTF/SourceSans3-Black.ttf?raw=true",
    "https://github.com/adobe-fonts/source-sans/blob/release/TTF/SourceSans3-Bold.ttf?raw=true",
    "https://github.com/adobe-fonts/source-sans/blob/release/TTF/SourceSans3-ExtraLight.ttf?raw=true",
    "https://github.com/adobe-fonts/source-sans/blob/release/TTF/SourceSans3-Light.ttf?raw=true",
    "https://github.com/adobe-fonts/source-sans/blob/release/TTF/SourceSans3-Regular.ttf?raw=true",
    "https://github.com/adobe-fonts/source-sans/blob/release/TTF/SourceSans3-Semibold.ttf?raw=true",
    "https://github.com/googlefonts/dm-fonts/blob/main/Sans/Exports/DMSans-Bold.ttf?raw=true",
    "https://github.com/googlefonts/dm-fonts/blob/main/Sans/Exports/DMSans-Medium.ttf?raw=true",
    "https://github.com/googlefonts/dm-fonts/blob/main/Sans/Exports/DMSans-Regular.ttf?raw=true",
    "https://github.com/googlefonts/opensans/blob/main/fonts/ttf/OpenSans-Bold.ttf?raw=true",
    "https://github.com/googlefonts/opensans/blob/main/fonts/ttf/OpenSans-ExtraBold.ttf?raw=true",
    "https://github.com/googlefonts/opensans/blob/main/fonts/ttf/OpenSans-Light.ttf?raw=true",
    "https://github.com/googlefonts/opensans/blob/main/fonts/ttf/OpenSans-Regular.ttf?raw=true",
    "https://github.com/googlefonts/opensans/blob/main/fonts/ttf/OpenSans-SemiBold.ttf?raw=true",
    "https://github.com/googlefonts/MetrophobicFont/blob/main/fonts/Metrophobic-Regular.ttf?raw=true",
    # "https://github.com/googlefonts/moolahlah/blob/master/fonts/ttf/MooLahLah-Regular.ttf?raw=true",
    # "https://github.com/googlefonts/island-moments/blob/master/fonts/ttf/IslandMoments-Regular.ttf?raw=true",
    # "https://github.com/googlefonts/licorice/blob/master/fonts/ttf/Licorice-Regular.ttf?raw=true",
    # "https://github.com/googlefonts/luxurious/blob/master/fonts/ttf/LuxuriousScript-Regular.ttf?raw=true",
    "https://github.com/googlefonts/fuzzy-bubbles/blob/master/fonts/ttf/FuzzyBubbles-Bold.ttf?raw=true",
    "https://github.com/googlefonts/fuzzy-bubbles/blob/master/fonts/ttf/FuzzyBubbles-Regular.ttf?raw=true",
    "https://github.com/googlefonts/kings/blob/master/fonts/ttf/Kings-Regular.ttf",
    # "https://github.com/googlefonts/petemoss/blob/master/fonts/ttf/Petemoss-Regular.ttf",
    # "https://github.com/googlefonts/estonia/blob/master/fonts/ttf/Estonia-Regular.ttf",
    # "https://github.com/googlefonts/corinthia/blob/master/fonts/ttf/Corinthia-Bold.ttf",
    # "https://github.com/googlefonts/corinthia/blob/master/fonts/ttf/Corinthia-Regular.ttf",
    "https://github.com/googlefonts/Signika/blob/master/fonts/ttf/Signika-Bold.ttf",
    "https://github.com/googlefonts/Signika/blob/master/fonts/ttf/Signika-Light.ttf",
    "https://github.com/googlefonts/Signika/blob/master/fonts/ttf/Signika-Medium.ttf",
    "https://github.com/googlefonts/Signika/blob/master/fonts/ttf/Signika-Regular.ttf",
    "https://github.com/googlefonts/Signika/blob/master/fonts/ttf/Signika-Semibold.ttf",
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-Black.ttf",
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-Bold.ttf",
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-ExtraBold.ttf",
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-ExtraLight.ttf",
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-Light.ttf",
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-Medium.ttf",
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-Regular.ttf",
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-SemiBold.ttf",
    "https://github.com/googlefonts/alumni/blob/master/fonts/ttf/AlumniSans-Thin.ttf",
    "https://github.com/googlefonts/ibarrareal/blob/main/fonts/ttf/IbarraRealNova-Bold.ttf",
    "https://github.com/googlefonts/ibarrareal/blob/main/fonts/ttf/IbarraRealNova-Regular.ttf",
    "https://github.com/googlefonts/ibarrareal/blob/main/fonts/ttf/IbarraRealNova-SemiBold.ttf",
    "https://github.com/googlefonts/gideon/blob/master/fonts/ttf/GideonRoman-Regular.ttf",
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-Black.ttf",
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-Bold.ttf",
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-ExtraBold.ttf",
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-ExtraLight.ttf",
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-Light.ttf",
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-Medium.ttf",
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-Regular.ttf",
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-SemiBold.ttf",
    "https://github.com/Outfitio/Outfit-Fonts/blob/main/fonts/ttf/Outfit-Thin.ttf",
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-Black.ttf",
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-Bold.ttf",
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-ExtraBold.ttf",
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-ExtraLight.ttf",
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-Light.ttf",
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-Medium.ttf",
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-Regular.ttf",
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-SemiBold.ttf",
    "https://github.com/googlefonts/LatoGFVersion/blob/main/fonts/Lato-Thin.ttf",
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-Black.ttf",
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-Bold.ttf",
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-ExtraLight.ttf",
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-Light.ttf",
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-Medium.ttf",
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-Regular.ttf",
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-SemiBold.ttf",
    "https://github.com/JulietaUla/Montserrat/blob/master/fonts/ttf/Montserrat-Thin.ttf",
    "https://github.com/itfoundry/Poppins/blob/master/variable/TTF%20(Beta)/Poppins-VariableFont_wght.ttf",
    "https://github.com/clauseggers/Playfair-Display/blob/master/fonts/TTF/PlayfairDisplay-Black.ttf",
    "https://github.com/clauseggers/Playfair-Display/blob/master/fonts/TTF/PlayfairDisplay-Bold.ttf",
    "https://github.com/clauseggers/Playfair-Display/blob/master/fonts/TTF/PlayfairDisplay-Regular.ttf",
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-Black.ttf",
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-Bold.ttf",
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-ExtraBold.ttf",
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-Light.ttf",
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-Medium.ttf",
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-Regular.ttf",
    "https://github.com/googlefonts/rubik/blob/main/fonts/ttf/Rubik-SemiBold.ttf",
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-Black.ttf",
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-Bold.ttf",
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-ExtraBold.ttf",
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-ExtraLight.ttf",
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-Light.ttf",
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-Medium.ttf",
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-Regular.ttf",
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-SemiBold.ttf",
    "https://github.com/weiweihuanghuang/Work-Sans/blob/master/fonts/ttf/WorkSans-Thin.ttf",
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-Black.ttf",
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-Bold.ttf",
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-ExtraBold.ttf",
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-Light.ttf",
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-Medium.ttf",
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-Regular.ttf",
    "https://github.com/OdedEzer/heebo/blob/master/fonts/ttf/Heebo-Thin.ttf",
    # "https://github.com/impallari/Libre-Franklin/blob/master/fonts/TTF/LibreFranklin-Black.ttf",
    # "https://github.com/impallari/Libre-Franklin/blob/master/fonts/TTF/LibreFranklin-Bold.ttf",
    # "https://github.com/impallari/Libre-Franklin/blob/master/fonts/TTF/LibreFranklin-ExtraBold.ttf",
    # "https://github.com/impallari/Libre-Franklin/blob/master/fonts/TTF/LibreFranklin-ExtraLight.ttf",
    # "https://github.com/impallari/Libre-Franklin/blob/master/fonts/TTF/LibreFranklin-Light.ttf",
    # "https://github.com/impallari/Libre-Franklin/blob/master/fonts/TTF/LibreFranklin-Medium.ttf",
    # "https://github.com/impallari/Libre-Franklin/blob/master/fonts/TTF/LibreFranklin-Regular.ttf",
    # "https://github.com/impallari/Libre-Franklin/blob/master/fonts/TTF/LibreFranklin-SemiBold.ttf",
    # "https://github.com/impallari/Libre-Franklin/blob/master/fonts/TTF/LibreFranklin-Thin.ttf"
]

fonts.reverse()
fontnum = len(fonts)
variationnum = fontnum * 3

for fonturl in fonts:
    for prop in [(38, -10), (34, -8), (30, -6)]:
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
