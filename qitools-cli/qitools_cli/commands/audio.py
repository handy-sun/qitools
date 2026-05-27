"""Audio processing commands."""

import click
from pathlib import Path
from qitools_cli.core.audio_processor import get_audio_metadata, set_audio_metadata, extract_cover
from qitools_cli.utils.output import print_success, print_error, print_table


@click.group(name="audio")
def audio_group():
    """Audio metadata processing tools."""
    pass


@audio_group.command(name="metadata")
@click.option("--file", "-f", "audio_file", required=True, type=click.Path(exists=True), help="Audio file")
@click.option("--set-title", help="Set title metadata")
@click.option("--set-artist", help="Set artist metadata")
@click.option("--set-album", help="Set album metadata")
def metadata(audio_file: str, set_title: str, set_artist: str, set_album: str):
    """Get or set audio metadata."""
    try:
        if set_title or set_artist or set_album:
            # Set metadata
            set_audio_metadata(audio_file, set_title, set_artist, set_album)
            print_success(f"Updated metadata for {audio_file}")
        else:
            # Get metadata
            meta = get_audio_metadata(audio_file)
            print_table("Audio Metadata", ["Key", "Value"], [[k, v] for k, v in meta.items()])
    except Exception as e:
        print_error(f"Error: {e}")
        raise click.Exit(1)


@audio_group.command(name="extract-cover")
@click.option("--file", "-f", "audio_file", required=True, type=click.Path(exists=True), help="Audio file")
@click.option("--output", "-o", "output_file", type=click.Path(), help="Output image file")
def extract_cover_cmd(audio_file: str, output_file: str):
    """Extract cover art from audio file."""
    try:
        if not output_file:
            # Auto-generate filename
            input_path = Path(audio_file)
            output_file = str(input_path.with_suffix('.jpg'))

        extract_cover(audio_file, output_file)
        print_success(f"Extracted cover to {output_file}")
    except Exception as e:
        print_error(f"Error: {e}")
        raise click.Exit(1)
