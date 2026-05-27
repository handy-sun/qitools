"""Image conversion commands."""

import click
from pathlib import Path
from qitools_cli.core.image_converter import convert_image, resize_image
from qitools_cli.utils.output import print_success, print_error


@click.group(name="image")
def image_group():
    """Image conversion and processing tools."""
    pass


@image_group.command(name="convert")
@click.option("--input", "-i", "input_file", required=True, type=click.Path(exists=True), help="Input image file")
@click.option("--format", "-f", "output_format", required=True, help="Output format (jpg/png/bmp/pgm/pbm)")
@click.option("--output", "-o", "output_file", type=click.Path(), help="Output file")
def convert(input_file: str, output_format: str, output_file: str):
    """Convert image format."""
    try:
        if not output_file:
            # Auto-generate output filename
            input_path = Path(input_file)
            output_file = str(input_path.with_suffix(f".{output_format}"))

        convert_image(input_file, output_file, output_format)
        print_success(f"Converted to {output_file}")
    except Exception as e:
        print_error(f"Error: {e}")
        raise click.Exit(1)


@image_group.command(name="resize")
@click.option("--input", "-i", "input_file", required=True, type=click.Path(exists=True), help="Input image file")
@click.option("--width", "-w", type=int, help="Target width")
@click.option("--height", "-h", type=int, help="Target height")
@click.option("--keep-aspect", is_flag=True, help="Keep aspect ratio")
@click.option("--output", "-o", "output_file", type=click.Path(), help="Output file")
def resize(input_file: str, width: int, height: int, keep_aspect: bool, output_file: str):
    """Resize image."""
    try:
        if not output_file:
            # Auto-generate output filename
            input_path = Path(input_file)
            output_file = str(input_path.with_name(f"{input_path.stem}_resized{input_path.suffix}"))

        resize_image(input_file, output_file, width, height, keep_aspect)
        print_success(f"Resized to {output_file}")
    except Exception as e:
        print_error(f"Error: {e}")
        raise click.Exit(1)
