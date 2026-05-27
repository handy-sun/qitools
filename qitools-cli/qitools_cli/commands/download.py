"""Download commands."""

import click
from pathlib import Path
from qitools_cli.core.downloader import download_file
from qitools_cli.utils.output import print_success, print_error


@click.group(name="download")
def download_group():
    """HTTP/HTTPS download tools."""
    pass


@download_group.command(name="file")
@click.option("--url", "-u", required=True, help="URL to download")
@click.option("--output", "-o", "output_file", type=click.Path(), help="Output file")
def download(url: str, output_file: str):
    """Download a file."""
    try:
        if not output_file:
            # Auto-generate filename from URL
            from urllib.parse import urlparse
            parsed = urlparse(url)
            output_file = Path(parsed.path).name or "download"

        download_file(url, output_file)
        print_success(f"Downloaded to {output_file}")
    except Exception as e:
        print_error(f"Error: {e}")
        raise click.Exit(1)
