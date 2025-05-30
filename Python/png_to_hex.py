from PIL import Image

def convert_png_to_hex(filename):
    # Bild laden und sicherstellen, dass es 1-Bit monochrom ist
    image = Image.open(filename).convert('1')

    width, height = image.size
    if width != 128 or height != 64:
        raise ValueError("Bild muss exakt 128x64 Pixel groß sein.")

    pixels = image.load()

    hex_data = []

    # Zeilenweise jeweils 8 vertikale Pixel pro Spalte bündeln
    for page in range(0, height // 8):  # 8 Seiten à 8 Pixel Höhe
        for x in range(width):
            byte = 0
            for bit in range(8):
                y = page * 8 + bit
                if pixels[x, y] == 0:  # 0 = schwarz (Pixel gesetzt)
                    byte |= (1 << bit)
            hex_data.append(byte)

    return hex_data

def print_hex_array(hex_data, bytes_per_line=16):
    print("const uint8_t image_data[] PROGMEM = {")
    for i in range(0, len(hex_data), bytes_per_line):
        chunk = hex_data[i:i+bytes_per_line]
        line = " ".join(f"{b:02X}" for b in chunk)
        print("  " + line)
    print("};")

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 2:
        print("Nutzung: python png_to_hex.py bild.png")
        sys.exit(1)

    hex_data = convert_png_to_hex(sys.argv[1])
    print_hex_array(hex_data)
