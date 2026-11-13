#!/usr/bin/env python3
"""Generate a static C diffuse map from a PNG or JPG image.

Requires Pillow: python3 -m pip install Pillow
"""

import argparse
import re
from pathlib import Path

try:
    from PIL import Image
except ImportError as exc:
    raise SystemExit(
        "Pillow is required to generate diffuse maps. Install with: "
        "python3 -m pip install Pillow"
    ) from exc


def identifier(value: str) -> str:
    result = re.sub(r"[^A-Za-z0-9_]", "_", value)
    if not result or result[0].isdigit():
        result = "_" + result
    return result


def linearize(channel: int) -> float:
    srgb = channel / 255.0
    if srgb <= 0.04045:
        return srgb / 12.92
    return ((srgb + 0.055) / 1.055) ** 2.4


def c_float(value: float) -> str:
    literal = format(value, ".9g")
    if "." not in literal and "e" not in literal.lower():
        literal += ".0"
    return literal + "f"


def generate(source: Path, destination: Path) -> None:
    if source.suffix.lower() not in {".png", ".jpg"}:
        raise ValueError(f"unsupported image format: {source.suffix}")

    stem = identifier(source.stem)
    type_name = f"DiffuseMap{stem}"
    array_name = f"diffuseMap{stem}Data"
    instance_name = f"diffuseMap{stem}"
    guard = f"TRAYRACING_GENERATED_DIFFUSE_MAP_{stem.upper()}_H"

    with Image.open(source) as image:
        rgb = image.convert("RGB")
        width, height = rgb.size
        pixels = list(rgb.getdata())

    lines = [
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        '#include "trayracing/trayracing.h"',
        "",
        f"typedef struct {type_name} {{",
        "    uint32_t width;",
        "    uint32_t height;",
        "    Vec3 const *data;",
        f"}} {type_name};",
        "",
        f"static const Vec3 {array_name}[{len(pixels)}] = {{",
    ]
    for r, g, b in pixels:
        lines.append(
            f"    {{{c_float(linearize(r))}, {c_float(linearize(g))}, "
            f"{c_float(linearize(b))}}},"
        )
    lines.extend([
        "};",
        f"static const {type_name} {instance_name} = {{",
        f"    {width}u, {height}u, {array_name}",
        "};",
        "",
        f"#endif /* {guard} */",
        "",
    ])

    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text("\n".join(lines), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("image", type=Path, help="source PNG or JPG image")
    parser.add_argument("output", type=Path, help="generated C header path")
    args = parser.parse_args()
    generate(args.image, args.output)


if __name__ == "__main__":
    main()
