"""Color conversion commands."""

import click
from qitools_cli.core.color_converter import parse_color, format_color
from qitools_cli.utils.output import print_error, print_table


@click.group(name="color")
def color_group():
    """Color format conversion tools."""
    pass


@color_group.command(name="parse")
@click.option("--hex", "hex_color", help="Hex color (e.g., #FF0000)")
@click.option("--rgb", "rgb_color", help="RGB color (e.g., 255,0,0)")
@click.option("--dec", "dec_color", help="Decimal color (e.g., 16711680)")
@click.option("--gl", "gl_color", help="GL color (e.g., 1.0,0.0,0.0)")
@click.option("--format", "-f", "output_format", default="all", help="Output format (rgb/hex/dec/gl/all)")
def parse(hex_color: str, rgb_color: str, dec_color: str, gl_color: str, output_format: str):
    """Parse and convert color formats."""
    try:
        # Parse input color
        r, g, b = parse_color(hex_color, rgb_color, dec_color, gl_color)

        # Format output
        result = format_color(r, g, b, output_format)

        if output_format == "all":
            print_table("Color Formats", ["Format", "Value"], result)
        else:
            click.echo(result)
    except Exception as e:
        print_error(f"Error: {e}")
        raise click.Exit(1)
