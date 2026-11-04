#!/usr/bin/env python3
# Writes assets/textures/player.png, a placeholder suit for the cube player. Run from the repo root.
import random
import struct
import zlib

WIDTH, HEIGHT = 256, 512  # powers of two, and close to the cube's 0.8 x 1.7 side
SUIT, DARK, ORANGE, VISOR = (215, 215, 205), (60, 62, 68), (230, 120, 30), (20, 30, 45)


def color_at(x, y):
    if 50 <= y < 105 and 40 <= x < 216:
        return (70, 90, 115) if y < 62 else VISOR
    if 140 <= y < 150 or 300 <= y < 320 or y >= 450:
        return DARK
    if 190 <= y < 225:
        return ORANGE
    if y >= 320 and 122 <= x < 134:
        return DARK
    return SUIT


def png_chunk(tag, data):
    return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data))


rng = random.Random(1)  # fixed seed, so every run writes the same file
rows = bytearray()
for y in range(HEIGHT):
    rows.append(0)
    for x in range(WIDTH):
        grain = rng.randint(-6, 6)
        # GenMeshCube's sides have v = 1 at the top, so the file is stored upside down
        rows.extend(max(0, min(255, c + grain)) for c in color_at(x, HEIGHT - 1 - y))

with open("assets/textures/player.png", "wb") as f:
    f.write(b"\x89PNG\r\n\x1a\n")
    f.write(png_chunk(b"IHDR", struct.pack(">IIBBBBB", WIDTH, HEIGHT, 8, 2, 0, 0, 0)))
    f.write(png_chunk(b"IDAT", zlib.compress(bytes(rows), 9)))
    f.write(png_chunk(b"IEND", b""))
