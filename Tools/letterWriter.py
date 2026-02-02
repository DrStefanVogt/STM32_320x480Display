from PIL import Image, ImageDraw, ImageFont
import os
import string

BMP_DIR = "bmps"
TXT_FILE = "arrays_and_hex.txt"
SIZE = (8, 8)
LETTERS = ["M"]

os.makedirs(BMP_DIR, exist_ok=True)

def load_font():
    # Versuche gut lesbare Monospace-Schrift
    schriftart = "Tiny5-Regular.ttf"
    try:
        return ImageFont.truetype(schriftart, 12)
    except IOError:
        print(f"Warnung: {schriftart} nicht gefunden – Fallback-Font")
        return ImageFont.load_default()

def create_bmp(letter, font):
    img = Image.new("1", SIZE, 0)
    draw = ImageDraw.Draw(img)

    bbox = draw.textbbox((0, 0), letter, font=font)
    w = bbox[2] - bbox[0]
    h = bbox[3] - bbox[1]

    x = 1
    y = -3 # minimal nach oben für bessere Lesbarkeit

    draw.text((x, y), letter, 1, font=font)

    path = os.path.join(BMP_DIR, f"{letter}.bmp")
    img.save(path)
    return path

def createNO_bmp(letter, font):
    path = os.path.join(BMP_DIR, f"{letter}.bmp")
    return path

def bmp_to_bool_array(path):
    img = Image.open(path).convert("1")
    pixels = img.load()

    return [
        [bool(pixels[x, y]) for x in range(8)]
        for y in range(8)
    ]

def array_to_hex16(array):
    hex_values = []
    for i in range(0, 8, 2):
        bits = ""
        for row in (array[i], array[i + 1]):
            bits += "".join("1" if b else "0" for b in row)
        hex_values.append(f"0x{int(bits, 2):04X}")
    return hex_values

def array_to_hex32(array):
    hex_values = []
    for i in range(0, 8, 4):
        bits = ""
        for row in array[i:i + 4]:
            bits += "".join("1" if b else "0" for b in row)
        hex_values.append(f"0x{int(bits, 2):08X}")
    return hex_values


def main():
    font = load_font()
    results = {}

    # BMPs erzeugen & einlesen
    for letter in LETTERS:
        bmp_path = createNO_bmp(letter, font)
        array = bmp_to_bool_array(bmp_path)
        hex32 = array_to_hex32(array)
        results[letter] = (array, hex32)

    # Ausgabe in TXT
    with open(TXT_FILE, "w") as f:
        for letter, (array, hex32) in results.items():
            f.write(f"[\'{letter}\'] = {{ .rows ={{")
            # for row in array:
            #     f.write(" ".join("1" if v else "0" for v in row) + "\n")

            #f.write(f"Hex (2 x 32-bit):\n")
            f.write(", ".join(hex32) + "} },\n")

    print("Fertig!")
    print(f"- BMPs: {BMP_DIR}/")
    print(f"- Ausgabe: {TXT_FILE}")

if __name__ == "__main__":
    main()
