#!/usr/bin/env python3
"""
Doxygen Documentation Parser for Daedalus

Library for parsing Doxygen comments from C header files.
Generates beautiful HTML API documentation.

Usage:
    from doxygen_docs import parse_header_file, generate_api_docs_html
"""

import re
from pathlib import Path
from typing import List, Dict, Optional, Tuple


def parse_doxygen_comment(lines: List[str], start_idx: int) -> Tuple[Optional[Dict], int]:
    """
    Parse a Doxygen comment block starting at the given index.

    Returns: (parsed_doc_dict, end_index) or (None, start_idx) if not a doc comment
    """
    if start_idx >= len(lines):
        return None, start_idx

    line = lines[start_idx].strip()

    # Check if this is a Doxygen comment (/** or /*!)
    if not (line.startswith('/**') or line.startswith('/*!')):
        return None, start_idx

    doc = {
        'brief': '',
        'description': '',
        'params': [],
        'returns': '',
        'note': '',
        'raw_lines': []
    }

    idx = start_idx
    in_comment = True

    while idx < len(lines) and in_comment:
        line = lines[idx].strip()
        doc['raw_lines'].append(line)

        # End of comment
        if '*/' in line:
            in_comment = False
            idx += 1
            break

        # Remove leading * and whitespace
        line = re.sub(r'^\s*\*\s?', '', line)
        line = re.sub(r'^/\*\*\s*', '', line)

        # Parse @brief
        if line.startswith('@brief'):
            doc['brief'] = line[6:].strip()
        # Parse @param
        elif line.startswith('@param'):
            match = re.match(r'@param\s+(\w+)\s+(.+)', line)
            if match:
                doc['params'].append({
                    'name': match.group(1),
                    'desc': match.group(2)
                })
        # Parse @return or @returns
        elif line.startswith(('@return', '@returns')):
            doc['returns'] = re.sub(r'@returns?\s+', '', line)
        # Parse @note
        elif line.startswith('@note'):
            doc['note'] = line[5:].strip()
        # Continuation lines
        elif line and not line.startswith('*'):
            if doc['note']:
                doc['note'] += '\n' + line
            elif doc['returns']:
                doc['returns'] += '\n' + line
            elif doc['brief']:
                if doc['description']:
                    doc['description'] += '\n' + line
                else:
                    doc['description'] = line

        idx += 1

    return doc, idx


def parse_function_signature(line: str) -> Optional[Dict]:
    """
    Parse a function signature line.

    Returns: dict with 'return_type', 'name', 'params' or None
    """
    # Match: return_type function_name( params );
    match = re.match(r'^\s*(\w+(?:\s*\*)?)\s+(\w+)\s*\((.*?)\)\s*;', line)
    if not match:
        return None

    return {
        'return_type': match.group(1).strip(),
        'name': match.group(2),
        'params_raw': match.group(3).strip()
    }


def parse_header_file(header_path: Path) -> List[Dict]:
    """
    Parse the header file and extract all documented functions.

    Returns: List of function documentation dicts
    """
    with open(header_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    functions = []
    idx = 0

    while idx < len(lines):
        # Look for doxygen comment
        doc, idx = parse_doxygen_comment(lines, idx)

        if doc:
            # Next non-empty line should be the function signature
            while idx < len(lines):
                line = lines[idx].strip()
                if line and not line.startswith('//'):
                    func = parse_function_signature(line)
                    if func:
                        func['doc'] = doc
                        functions.append(func)
                    break
                idx += 1

        idx += 1

    return functions


def categorize_functions(functions: List[Dict]) -> Dict[str, List[Dict]]:
    """
    Categorize functions by object type and variant.

    Returns: Dict with categories like "Array (Dynamic)", "Array (Static)", etc.
    """
    categories = {}

    for func in functions:
        name = func['name']

        # Extract object type from function name (remove d_ prefix)
        if not name.startswith('d_'):
            continue

        name_without_prefix = name[2:]  # Remove "d_"

        # Determine object type and variant
        if 'StaticArray' in name_without_prefix:
            category = "Array (Static)"
        elif 'Array' in name_without_prefix and 'StaticArray' not in name_without_prefix:
            category = "Array (Dynamic)"
        elif 'StaticTable' in name_without_prefix:
            category = "Table (Static)"
        elif 'Table' in name_without_prefix and 'StaticTable' not in name_without_prefix:
            category = "Table (Dynamic)"
        elif 'String' in name_without_prefix:
            category = "String"
        elif 'LinkedList' in name_without_prefix or 'List' in name_without_prefix:
            category = "Linked List"
        elif 'DUF' in name_without_prefix:
            category = "DUF (Data Format)"
        elif 'Log' in name_without_prefix:
            category = "Logging"
        elif name_without_prefix.startswith(('Vec', 'Mat', 'Kinematic')):
            category = "Math & Physics"
        elif 'Hash' in name_without_prefix or 'Compare' in name_without_prefix:
            category = "Utility Functions"
        else:
            category = "Other"

        if category not in categories:
            categories[category] = []
        categories[category].append(func)

    return categories


def generate_api_docs_html(functions: List[Dict], project_root: Path, title: str = "Daedalus API Reference") -> None:
    """
    Generate a beautiful dark-mode HTML API reference with categorization.

    Args:
        functions: List of parsed function dictionaries
        project_root: Path to project root directory
        title: Title for the documentation page
    """
    # Categorize functions
    categories = categorize_functions(functions)
    html = f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>{title}</title>
    <style>
        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}
        body {{
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Roboto', 'Inter', sans-serif;
            background: #0d1117;
            color: #c9d1d9;
            padding: 40px 20px;
            line-height: 1.6;
        }}
        .container {{
            max-width: 1200px;
            margin: 0 auto;
        }}
        header {{
            margin-bottom: 48px;
            border-bottom: 1px solid #30363d;
            padding-bottom: 24px;
        }}
        h1 {{
            font-size: 2.5em;
            font-weight: 600;
            color: #f0f6fc;
            margin-bottom: 8px;
            letter-spacing: -0.03em;
        }}
        .subtitle {{
            font-size: 1.1em;
            color: #8b949e;
            font-weight: 400;
        }}
        .function {{
            background: #161b22;
            border: 1px solid #30363d;
            border-radius: 6px;
            padding: 24px;
            margin-bottom: 24px;
            transition: border-color 0.2s;
        }}
        .function:hover {{
            border-color: #58a6ff;
        }}
        .function-signature {{
            font-family: 'SF Mono', 'Monaco', 'Consolas', monospace;
            font-size: 1.1em;
            color: #79c0ff;
            margin-bottom: 16px;
            padding: 16px;
            background: #0d1117;
            border-radius: 6px;
            border: 1px solid #30363d;
            overflow-x: auto;
        }}
        .return-type {{
            color: #ff7b72;
        }}
        .function-name {{
            color: #d2a8ff;
            font-weight: 600;
        }}
        .param {{
            color: #ffa657;
        }}
        .brief {{
            font-size: 1.05em;
            color: #c9d1d9;
            margin-bottom: 16px;
            line-height: 1.6;
        }}
        .section-title {{
            font-size: 0.9em;
            font-weight: 600;
            color: #8b949e;
            text-transform: uppercase;
            letter-spacing: 0.05em;
            margin-top: 16px;
            margin-bottom: 8px;
        }}
        .param-list {{
            list-style: none;
        }}
        .param-item {{
            padding: 8px 0;
            border-bottom: 1px solid #21262d;
        }}
        .param-item:last-child {{
            border-bottom: none;
        }}
        .param-name {{
            font-family: 'SF Mono', 'Monaco', 'Consolas', monospace;
            color: #ffa657;
            font-weight: 600;
        }}
        .param-desc {{
            color: #8b949e;
            margin-left: 8px;
        }}
        .returns {{
            color: #8b949e;
            padding: 12px;
            background: #0d1117;
            border-radius: 6px;
            border-left: 3px solid #58a6ff;
        }}
        .note {{
            color: #8b949e;
            padding: 12px;
            background: #0d1117;
            border-radius: 6px;
            border-left: 3px solid #d29922;
            font-style: italic;
        }}
        .stats {{
            margin-top: 48px;
            padding: 24px;
            background: #161b22;
            border: 1px solid #30363d;
            border-radius: 6px;
            text-align: center;
        }}
        .stats-title {{
            font-size: 1.1em;
            color: #f0f6fc;
            font-weight: 600;
            margin-bottom: 8px;
        }}
        .stats-value {{
            font-size: 2em;
            color: #58a6ff;
            font-weight: 700;
        }}
        .footer {{
            margin-top: 16px;
            color: #6e7681;
            font-size: 0.9em;
        }}
        .category {{
            margin-bottom: 48px;
        }}
        .category-header {{
            display: flex;
            align-items: center;
            gap: 12px;
            margin-bottom: 24px;
            padding-bottom: 12px;
            border-bottom: 2px solid #30363d;
        }}
        .category-title {{
            font-size: 1.8em;
            font-weight: 600;
            color: #f0f6fc;
        }}
        .category-count {{
            background: #30363d;
            color: #8b949e;
            padding: 4px 12px;
            border-radius: 12px;
            font-size: 0.75em;
            font-weight: 500;
        }}
        .category-emoji {{
            font-size: 1.5em;
        }}
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>📚 {title}</h1>
            <div class="subtitle">Complete function documentation • Grouped by object type</div>
        </header>
"""

    # Category order and emojis
    category_info = {
        "Array (Dynamic)": "🔄",
        "Array (Static)": "📦",
        "Table (Dynamic)": "🗂️",
        "Table (Static)": "🗄️",
        "String": "📝",
        "Linked List": "🔗",
        "DUF (Data Format)": "📋",
        "Logging": "📊",
        "Math & Physics": "🧮",
        "Utility Functions": "🛠️",
        "Other": "📚"
    }

    # Sort categories by predefined order
    sorted_categories = sorted(categories.items(),
                               key=lambda x: list(category_info.keys()).index(x[0])
                               if x[0] in category_info else 999)

    for category_name, funcs in sorted_categories:
        if not funcs:
            continue

        emoji = category_info.get(category_name, "📚")

        html += f"""
        <div class="category">
            <div class="category-header">
                <span class="category-emoji">{emoji}</span>
                <span class="category-title">{category_name}</span>
                <span class="category-count">{len(funcs)} functions</span>
            </div>
"""

        # Sort functions alphabetically within category
        for func in sorted(funcs, key=lambda f: f['name']):
            doc = func['doc']

            # Build function signature with syntax highlighting
            signature = f'<span class="return-type">{func["return_type"]}</span> '
            signature += f'<span class="function-name">{func["name"]}</span>('

            if func['params_raw']:
                signature += f'<span class="param">{func["params_raw"]}</span>'

            signature += ')'

            html += f"""
            <div class="function">
                <div class="function-signature">{signature}</div>
"""

            if doc['brief']:
                html += f"""                <div class="brief">{doc['brief']}</div>
"""

            if doc['params']:
                html += """                <div class="section-title">Parameters</div>
                <ul class="param-list">
"""
                for param in doc['params']:
                    html += f"""                    <li class="param-item">
                        <span class="param-name">{param['name']}</span>
                        <span class="param-desc">{param['desc']}</span>
                    </li>
"""
                html += """                </ul>
"""

            if doc['returns']:
                html += f"""                <div class="section-title">Returns</div>
                <div class="returns">{doc['returns']}</div>
"""

            if doc['note']:
                html += f"""                <div class="section-title">Note</div>
                <div class="note">{doc['note']}</div>
"""

            html += """            </div>
"""

        html += """        </div>
"""

    html += f"""
        <div class="stats">
            <div class="stats-title">Documented Functions</div>
            <div class="stats-value">{len(functions)}</div>
            <div class="footer">Generated from Doxygen comments • Daedalus Library</div>
        </div>
    </div>
</body>
</html>
"""

    # Write HTML file
    output_dir = project_root / "architecture" / "documents"
    output_dir.mkdir(parents=True, exist_ok=True)
    output_path = output_dir / "api_reference.html"

    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(html)

    print(f"📚 API documentation generated: {output_path}")
    print(f"   Found {len(functions)} documented functions")
    print(f"   Open in browser to view!")
    print()


if __name__ == "__main__":
    # Can be run standalone to generate docs
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent
    header_file = project_root / "include" / "Daedalus.h"

    if header_file.exists():
        functions = parse_header_file(header_file)
        generate_api_docs_html(functions, project_root)
    else:
        print(f"Error: {header_file} not found")
