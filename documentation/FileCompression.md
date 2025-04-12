![FileCompression.png](images/FileCompression.png)

![C++](https://img.shields.io/badge/C++-20-blue)
![License](https://img.shields.io/badge/license-Apache_License_2.0.-green)
![Version](https://img.shields.io/badge/version-0.1-blue.svg)

## Overview
This component is a standalone, functional module that compresses an array (or blob) of files provided as a JSON document. When no compression method is specified, the system defaults to lossless LZMA compression. Users can select compatible options based on file type—for example, H.264 is offered only for video files and JPEG for images.  
There is an option for individual precompression for single files using additional parameters such as quality settings. For multiple files, they are aggregated and then compressed.  
*Note:* If the encryption flag is set to true in the JSON, an informative message ("Encryption is not yet implemented") will be returned.

## JSON / Blob Specification
- **Purpose:**  
  Serialize an array of files and compression options. This JSON structure will always be accepted, though future improvements may extend it to support additional settings and formats.
- **Required Fields for Each File:**
    - `fileName` – Name of the file.
    - `fileType` – The file extension (e.g., "txt", "mp4", "jpg").
    - `fileData` – Base64-encoded file content.
    - `lastModified` – Last modified date.
    - `creationDate` – Creation date.
- **Options Object:**
    - `compressionAlgorithm` – Choose among "LZMA" (default if missing), "zip", "H.264", "JPEG", "fast" (fastest available, benchmarked against Windows zipping speed).
    - `mode` – "lossless" or "lossy". (A lossy request prompts a confirmation message.)
    - `preCompress` – Object with settings for individual file types. For example:
        - `video`: `{ "compressionId": "H.264", "quality": 75 }`
        - `image`: `{ "compressionId": "JPEG", "quality": 80 }`
        - `text`: `null` (indicating no pre-compression)
    - `encryption`: Boolean value. If true, a message "Encryption is not yet implemented" will be returned.

### Example JSON Structure:
```json
{
  "files": [
    {
      "fileName": "video.mp4",
      "fileType": "mp4",
      "fileData": "Base64EncodedData...",
      "lastModified": "2023-10-10T15:30:00Z",
      "creationDate": "2023-09-01T12:00:00Z"
    },
    {
      "fileName": "image.jpg",
      "fileType": "jpg",
      "fileData": "Base64EncodedData...",
      "lastModified": "2023-10-08T10:20:00Z",
      "creationDate": "2023-08-15T08:00:00Z"
    }
  ],
  "options": {
    "compressionAlgorithm": "LZMA",
    "mode": "lossless",
    "preCompress": {
      "video": {"compressionId": "H.264", "quality": 75},
      "image": {"compressionId": "JPEG", "quality": 80},
      "text": null
    },
    "encryption": false
  }
}
```

## Functional Workflow
1. **Input Parsing:**  
   The API processes the entire JSON, verifying each file’s attributes and collecting global options.
2. **Error Handling:**  
   All errors and warnings are managed by a centralized error handler. For instance, unsupported file types trigger an automatic notification and a prompt for whether to continue with remaining files.
3. **Compression Processing:**  
   - **Single File:** Additional parameters may be applied (e.g., quality for videos).
   - **Multiple Files:** Files are aggregated into one archive after optional individual pre-compression.
4. **Checksum and Seal Verification:**  
   Every compressed file is validated using checksums, and a seal is applied that logs the compression and decompression audit.

## Error Codes (Compression Specific)
The following table lists sample error and warning codes used in the compression component:

| **Code** | **Type**   | **Location** | **Description**                                                        |
|----------|------------|--------------|------------------------------------------------------------------------|
| **0**    | Success    | -            | Operation succeeded.                                                   |
| **w1**   | Warning    | u/s/n        | Minor issue detected (e.g., non-critical data mismatch).               |
| **w2**   | Warning    | u/s/n        | Deprecated parameter or format in use.                                 |
| **eu1**  | Error      | u            | User error: Permission denied on file upload.                          |
| **eu2**  | Error      | u            | User error: Invalid JSON format.                                       |
| **es1**  | Error      | s            | Server error: Compression failure on processing file(s).               |
| **es2**  | Error      | s            | Server error: Checksum or seal verification failed.                    |
| **en1**  | Error      | n            | Network error: Communication failure not attributed to user or server. |
| **ee1**  | Error      | s            | Encryption error (for when encryption is implemented in future).       |

*Notes:*  
- Warnings and errors include location markers: `u` (user-side), `s` (server-side), and `n` (network).
- Informational codes (e.g., i1, i2) are logged separately (only storing the last 100 entries).

## Testing Strategy
| **Test Case**                                                                          | **Planned/Implemented** | **Notes**                                                                               |
|----------------------------------------------------------------------------------------|-------------------------|-----------------------------------------------------------------------------------------|
| 100MB file with expected ~10% compression (5 tests; average should exceed 8%)          | Planned                 | Test on files sized between 25MB and 100MB.                                             |
| 250MB file with expected ~40% compression (5 tests; average should be above 35–38%)    | Planned                 | Multiple runs to ensure consistency.                                                    |
| 5GB file compression test (5 tests; single file copied then deleted after test)        | Planned                 | Real-life scenario test for large files.                                                |
| 15GB file compression test (5 tests; for uncompression, ensure similar size post-test) | Planned                 | Ensures integrity and expected compression ratios across compression and uncompression. |
| Error simulation: missing files, permission issues, invalid JSON, unsupported types    | Planned                 | Both user-side (eu) and server-side (es) errors will be tested.                         |
| Fast Compression Option benchmark (time tests vs. Windows zip performance)             | Planned                 | Validate performance improvements relative to standard zipping times.                   |
| Plugin architecture test (dynamic registration of additional compression methods)      | Planned                 | Ensures fallback behavior if a plugin is not found.                                     |

## Usage Example (C++)
```cpp
#include "CompressionAPI.h"

int main() {
    std::string inputBlob = R"({
      "files": [
          {
              "fileName": "doc.txt",
              "fileType": "txt",
              "fileData": "Base64EncodedData...",
              "lastModified": "2023-10-10T15:30:00Z",
              "creationDate": "2023-09-01T12:00:00Z"
          },
          {
              "fileName": "video.mp4",
              "fileType": "mp4",
              "fileData": "Base64EncodedData...",
              "lastModified": "2023-10-08T10:20:00Z",
              "creationDate": "2023-08-15T08:00:00Z"
          }
      ],
      "options": {
          "compressionAlgorithm": "LZMA",
          "mode": "lossless",
          "preCompress": {
              "video": {"compressionId": "H.264", "quality": 75},
              "image": {"compressionId": "JPEG", "quality": 80},
              "text": null
          },
          "encryption": false
      }
    })";

    auto result = CompressionAPI::compressBlob(inputBlob);
    if(result.errorCode != CompressionAPI::ErrorCode::SUCCESS) {
        std::cerr << "Compression error: " << result.errorCode << std::endl;
        return -1;
    }
    // Process result.compressedFile as needed.
    return 0;
}
```

## Future Improvements
- **Additional Single File Compression Parameters:**  
  (e.g., quality, bitrate, resolution for videos; image resizing options.)
- **Enhanced JSON Structure:**  
  Guidelines on extending the JSON format to support new parameters and alternative serialization formats.
- **Optional Encryption Parameters:**  
  Incorporate Initialization Vector (IV) and salt in the JSON for future encryption use.
- **Dynamic Plugin Installer:**  
  Allow dynamic download and registration of new compression methods.
- **Signature Verification and Audit Logging:**  
  Automatically seal compressed files with digital signatures and log all operations.

## License
Licensed under Apache License 2.0.
