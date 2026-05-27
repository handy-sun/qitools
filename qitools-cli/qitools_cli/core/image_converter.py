"""Image conversion logic - no UI dependencies."""

from PIL import Image


def convert_image(input_file: str, output_file: str, output_format: str) -> None:
    """Convert image format.

    Args:
        input_file: Path to input image
        output_file: Path to output image
        output_format: Output format (jpg/png/bmp/pgm/pbm)
    """
    img = Image.open(input_file)

    # Convert RGBA to RGB for JPEG
    if output_format.lower() in ('jpg', 'jpeg') and img.mode == 'RGBA':
        img = img.convert('RGB')

    # Map jpg to JPEG for Pillow
    format_mapping = {
        'jpg': 'JPEG',
        'jpeg': 'JPEG',
        'png': 'PNG',
        'bmp': 'BMP',
        'pgm': 'PGM',
        'pbm': 'PBM'
    }
    pillow_format = format_mapping.get(output_format.lower(), output_format.upper())

    img.save(output_file, format=pillow_format)


def resize_image(
    input_file: str,
    output_file: str,
    width: int | None = None,
    height: int | None = None,
    keep_aspect: bool = False
) -> None:
    """Resize image.

    Args:
        input_file: Path to input image
        output_file: Path to output image
        width: Target width (None to auto-calculate)
        height: Target height (None to auto-calculate)
        keep_aspect: Whether to keep aspect ratio
    """
    img = Image.open(input_file)

    if width is None and height is None:
        raise ValueError("At least one of width or height must be specified")

    if keep_aspect:
        if width and height:
            # Calculate to fit within bounds
            ratio = min(width / img.width, height / img.height)
            new_size = (int(img.width * ratio), int(img.height * ratio))
        elif width:
            ratio = width / img.width
            new_size = (width, int(img.height * ratio))
        else:
            ratio = height / img.height
            new_size = (int(img.width * ratio), height)
    else:
        # When not keeping aspect ratio, use provided dimensions or original
        new_width = width if width else img.width
        new_height = height if height else img.height
        new_size = (new_width, new_height)

    img = img.resize(new_size, Image.Resampling.LANCZOS)
    img.save(output_file)
