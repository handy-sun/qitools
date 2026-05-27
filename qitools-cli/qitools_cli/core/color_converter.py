"""Color conversion logic - no UI dependencies."""


def parse_color(
    hex_color: str | None = None,
    rgb_color: str | None = None,
    dec_color: str | None = None,
    gl_color: str | None = None
) -> tuple[int, int, int]:
    """Parse color from various formats.

    Returns:
        Tuple of (r, g, b) values (0-255)
    """
    if hex_color:
        # Remove # if present
        hex_color = hex_color.lstrip('#')
        if len(hex_color) == 3:
            hex_color = ''.join(c * 2 for c in hex_color)
        r = int(hex_color[0:2], 16)
        g = int(hex_color[2:4], 16)
        b = int(hex_color[4:6], 16)
        return r, g, b

    if rgb_color:
        parts = rgb_color.split(',')
        if len(parts) != 3:
            raise ValueError("RGB format should be: r,g,b")
        return int(parts[0]), int(parts[1]), int(parts[2])

    if dec_color:
        dec = int(dec_color)
        r = (dec >> 16) & 0xFF
        g = (dec >> 8) & 0xFF
        b = dec & 0xFF
        return r, g, b

    if gl_color:
        parts = gl_color.split(',')
        if len(parts) != 3:
            raise ValueError("GL format should be: r,g,b (0.0-1.0)")
        r = int(float(parts[0]) * 255)
        g = int(float(parts[1]) * 255)
        b = int(float(parts[2]) * 255)
        return r, g, b

    raise ValueError("No color input provided")


def format_color(r: int, g: int, b: int, output_format: str) -> str | list[list[str]]:
    """Format color to specified format.

    Returns:
        Formatted string or list of [format, value] pairs for 'all'
    """
    if output_format == "hex":
        return f"#{r:02X}{g:02X}{b:02X}"

    if output_format == "rgb":
        return f"{r},{g},{b}"

    if output_format == "dec":
        return str((r << 16) | (g << 8) | b)

    if output_format == "gl":
        return f"{r/255:.3f},{g/255:.3f},{b/255:.3f}"

    if output_format == "all":
        return [
            ["Hex", f"#{r:02X}{g:02X}{b:02X}"],
            ["RGB", f"{r},{g},{b}"],
            ["Decimal", str((r << 16) | (g << 8) | b)],
            ["GL", f"{r/255:.3f},{g/255:.3f},{b/255:.3f}"],
        ]

    raise ValueError(f"Unknown format: {output_format}")
