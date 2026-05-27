"""Codec conversion commands."""

import click
from pathlib import Path
from qitools_cli.core.codec_converter import detect_encoding, convert_encoding
from qitools_cli.utils.output import print_success, print_error, print_info


@click.group(name="codec")
def codec_group():
    """Encoding detection and conversion tools."""
    pass


@codec_group.command(name="detect")
@click.option("--input", "-i", "input_file", required=True, type=click.Path(exists=True), help="Input file")
def detect(input_file: str):
    """Detect file encoding."""
    try:
        encoding = detect_encoding(input_file)
        print_info(f"Detected encoding: {encoding}")
    except Exception as e:
        print_error(f"Error: {e}")
        raise click.Exit(1)


@codec_group.command(name="convert")
@click.option("--input", "-i", "input_file", required=True, type=click.Path(exists=True), help="Input file")
@click.option("--from", "from_encoding", help="Source encoding (auto-detect if not specified)")
@click.option("--to", "to_encoding", required=True, help="Target encoding")
@click.option("--output", "-o", "output_file", type=click.Path(), help="Output file")
@click.option("--inplace", is_flag=True, help="Convert file in place")
def convert(input_file: str, from_encoding: str, to_encoding: str, output_file: str, inplace: bool):
    """Convert file encoding."""
    try:
        if not output_file and not inplace:
            print_error("Either --output or --inplace must be specified")
            raise click.Exit(1)

        result = convert_encoding(input_file, from_encoding, to_encoding)

        if inplace:
            Path(input_file).write_text(result)
            print_success(f"Converted {input_file} to {to_encoding}")
        else:
            Path(output_file).write_text(result)
            print_success(f"Converted to {output_file}")
    except Exception as e:
        print_error(f"Error: {e}")
        raise click.Exit(1)
