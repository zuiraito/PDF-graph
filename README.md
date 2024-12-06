# Usage
1. Save the pdf files into the Data directory.
2. Execute ./quick_launch.
   - this runs the backend to generate the json files for the graph and starts a local HTTP server on port 8000
3. A firefox tab should automatically open.
# Installation
## Dependencies
sudo pacman -S poppler
sudo pacman -S nlohmann-json
I will write something in here when i find the time. Its not that hard to fiqure it out on your own.
## Structure
```
.
├── Backend
│   ├── clear_jsons.sh
│   ├── combine_jsons.sh
│   ├── compile.sh
│   ├── ignore_words
│   ├── JSON_head_and_tail
│   │   ├── head
│   │   └── tail
│   ├── JSONs
│   │   ├── AMSP_SS2023_CH8_L15.json
│   │   ├── AMSP_SS2024_CH8_L16.json
│   │   ├── AMSP_SS2024_CH9_L17-PrimaryShaping.json
│   │   └── AMSP_SS2024_CH9_L18-Forming notes.json
│   ├── move_to_frontend.sh
│   ├── pdf_titles
│   ├── pdf_titles.cpp
│   └── run.sh
├── Data
│   ├── AMSP_SS2023_CH8_L15.pdf
│   ├── AMSP_SS2024_CH8_L16.pdf
│   ├── AMSP_SS2024_CH9_L17-PrimaryShaping.pdf
│   └── AMSP_SS2024_CH9_L18-Forming notes.pdf
├── Frontend
│   ├── graphData.json
│   ├── graph.js
│   ├── index.html
│   └── run.sh
├── launch.sh
├── quick_launch.sh
└── README.md
```
## Todo
- Add the compile scripts to replace manual compiling of the c++ code
- center selected nodes
- open the corresponding documen on the correct page, when a node is clicked
