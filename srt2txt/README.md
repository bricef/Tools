# SRT to TXT Converter

A simple web application that converts SRT (SubRip Subtitle) files to plain text transcripts by extracting only the subtitle text content and removing timing information and subtitle numbers.

## What it does

This tool takes SRT subtitle files and converts them to plain text by:
- Removing subtitle numbers (e.g., "1", "2", "3")
- Removing timing information (e.g., "00:00:01,000 --> 00:00:04,000")
- Extracting only the actual subtitle text content
- Combining all text into a single, readable transcript

## Features

- Web-based interface for easy file upload
- Real-time conversion
- Error handling for malformed SRT files
- Simple and clean output format

## Usage

### Web Interface

1. Open the web application in your browser
2. Click "Choose File" to select an SRT file
3. Click "Start upload" to convert the file
4. View the extracted text in the results area

### Example Input (SRT file)
```
1
00:00:01,000 --> 00:00:04,000
Hello, welcome to this video.

2
00:00:04,500 --> 00:00:07,500
Today we'll be discussing important topics.
```

### Example Output
```
Hello, welcome to this video.
Today we'll be discussing important topics.
```

## Technical Details

### Dependencies

- **Python 2.7** (as specified in app.yaml)
- **Bottle** - Lightweight web framework
- **Pystache** - Template engine for HTML rendering
- **Pysrt** - SRT file parsing library

### Project Structure

```
srt2txt/
├── main.py              # Main application logic
├── app.yaml             # Google App Engine configuration
├── requirements.txt     # Python dependencies
├── CNAME               # Custom domain configuration
├── templates/          # HTML templates
│   ├── index.html      # Upload form
│   ├── upload.html     # Results display
│   └── 404.html        # Error page
└── README.md           # This file
```

### Core Functions

- `parse(input)` - Parses SRT file and extracts text content
- `numerical_ok(line)` - Validates subtitle number format
- `timing_ok(line)` - Validates timing information format
- `render(filename, data)` - Renders HTML templates

## Deployment

### Local Development

1. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```

2. Run the application:
   ```bash
   python main.py
   ```

3. Access the application at `http://localhost:8080`

## Error Handling

The application includes error handling for:
- Malformed SRT files
- Invalid subtitle number formats
- Invalid timing information formats
- File upload issues

## License

This project is part of the Tools repository. See the main LICENSE file for details.
