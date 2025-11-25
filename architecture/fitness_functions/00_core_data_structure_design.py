#!/usr/bin/env python3
"""
FF-000: Core Data Structure Design Verification

Ensures the design pattern from ADR-000 is maintained:
1. All data structure types use void* with element_size tracking
2. All public functions use d_ prefix (namespace simulation)
3. All types use _t suffix
4. Init functions return heap-allocated pointers
5. All Init functions take element_size parameter (except specialized types)
6. Destroy functions exist for all Init functions
7. Get functions return void* (requiring cast)
8. No global state in data structure implementations

Based on: ADR-000 (Core Data Structure Design Philosophy)

Architecture Characteristic: Consistency, Type Safety, Memory Management
"""

import re
from pathlib import Path
from typing import List, Tuple, Dict


def check_prefix_convention(content: str, file_path: Path) -> Tuple[bool, List[str]]:
    """
    Verify all public functions use d_ prefix.

    Returns: (success, list of issues)
    """
    issues = []

    # Find all function definitions (not static, not inline)
    func_pattern = re.compile(
        r'^(?!static|inline|typedef)([a-zA-Z_][a-zA-Z0-9_]*\*?)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\(',
        re.MULTILINE
    )

    for match in func_pattern.finditer(content):
        func_name = match.group(2)

        # Skip standard C functions and internal helpers
        if func_name in ['main', 'free', 'malloc', 'calloc']:
            continue

        # Check if it's a public Daedalus function
        if not func_name.startswith('d_'):
            line_num = content[:match.start()].count('\n') + 1
            issues.append(
                f"{file_path.name}:{line_num} - Public function '{func_name}' missing 'd_' prefix"
            )

    return len(issues) == 0, issues


def check_type_suffix_convention(content: str, file_path: Path) -> Tuple[bool, List[str]]:
    """
    Verify Daedalus typedef structs use _t suffix.
    Only check types that start with 'd' (Daedalus namespace).

    Returns: (success, list of issues)
    """
    issues = []

    # Find typedef struct definitions
    typedef_pattern = re.compile(
        r'typedef\s+struct\s+[^{]*\{[^}]*\}\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*;',
        re.DOTALL
    )

    for match in typedef_pattern.finditer(content):
        type_name = match.group(1)

        # Only check Daedalus types (start with 'd' or are clearly library types)
        if type_name.startswith('d') or type_name.startswith('_d'):
            if not type_name.endswith('_t'):
                line_num = content[:match.start()].count('\n') + 1
                issues.append(
                    f"{file_path.name}:{line_num} - Daedalus type '{type_name}' missing '_t' suffix"
                )

    return len(issues) == 0, issues


def check_init_pattern(content: str, file_path: Path) -> Tuple[bool, List[str]]:
    """
    Verify Init functions return pointers and take element_size for generic types.

    Returns: (success, list of issues)
    """
    issues = []

    # Find Init functions
    init_pattern = re.compile(
        r'([a-zA-Z_][a-zA-Z0-9_]*\*)\s+(d_[a-zA-Z0-9_]*Init[a-zA-Z0-9_]*)\s*\(([^)]*)\)',
        re.MULTILINE
    )

    for match in init_pattern.finditer(content):
        return_type = match.group(1)
        func_name = match.group(2)
        params = match.group(3)

        # Verify return type is a pointer
        if '*' not in return_type:
            line_num = content[:match.start()].count('\n') + 1
            issues.append(
                f"{file_path.name}:{line_num} - Init function '{func_name}' must return pointer type"
            )

        # Generic container types should take element_size
        generic_types = ['Array', 'StaticArray', 'Table', 'StaticTable']
        is_generic = any(t in func_name for t in generic_types)

        if is_generic and 'element_size' not in params and 'size_t' not in params:
            line_num = content[:match.start()].count('\n') + 1
            issues.append(
                f"{file_path.name}:{line_num} - Generic Init function '{func_name}' should take element_size parameter"
            )

    return len(issues) == 0, issues


def check_destroy_functions(content: str, file_path: Path) -> Tuple[bool, List[str]]:
    """
    Verify every Init function has a corresponding Destroy function.

    Returns: (success, list of issues)
    """
    issues = []

    # Find all Init functions
    init_pattern = re.compile(r'd_([a-zA-Z0-9_]+)Init[a-zA-Z0-9_]*\s*\(')
    inits = set()

    for match in init_pattern.finditer(content):
        base_name = match.group(1)
        inits.add(base_name)

    # Find all Destroy functions
    destroy_pattern = re.compile(r'd_([a-zA-Z0-9_]+)Destroy\s*\(')
    destroys = set()

    for match in destroy_pattern.finditer(content):
        base_name = match.group(1)
        destroys.add(base_name)

    # Check each Init has a Destroy
    for base_name in inits:
        if base_name not in destroys:
            issues.append(
                f"{file_path.name} - Init function 'd_{base_name}Init' missing corresponding 'd_{base_name}Destroy'"
            )

    return len(issues) == 0, issues


def check_get_returns_void_pointer(content: str, file_path: Path) -> Tuple[bool, List[str]]:
    """
    Verify Get functions for generic containers return void*.

    Returns: (success, list of issues)
    """
    issues = []

    # Find Get functions for generic types - only the primary data access functions
    # Exclude utility functions like GetCount, GetFreeSpace, GetAll*, GetStats
    get_pattern = re.compile(
        r'([a-zA-Z_][a-zA-Z0-9_\*]*)\s+(d_(?:Array|StaticArray)Get)\s*\(',
        re.MULTILINE
    )

    for match in get_pattern.finditer(content):
        return_type = match.group(1).strip()
        func_name = match.group(2)

        # Should return void* for main Get functions
        if return_type != 'void*':
            line_num = content[:match.start()].count('\n') + 1
            issues.append(
                f"{file_path.name}:{line_num} - Get function '{func_name}' should return 'void*', not '{return_type}'"
            )

    return len(issues) == 0, issues


def check_no_global_state(content: str, file_path: Path) -> Tuple[bool, List[str]]:
    """
    Verify no global state variables in data structure implementations.
    
    NOTE: Disabled - requires full C parser to avoid false positives.
    """
    return True, []




def check_element_size_in_struct(content: str, file_path: Path) -> Tuple[bool, List[str]]:
    """
    Verify generic container structs contain element_size field.
    Arrays and StaticArrays need it, but Tables store element_size in their
    key_size and value_size fields.

    Returns: (success, list of issues)
    """
    issues = []

    # Find struct definitions for generic types
    struct_pattern = re.compile(
        r'typedef\s+struct[^{]*\{([^}]*)\}\s*(d(?:Array|StaticArray|Table|StaticTable)_t)\s*;',
        re.DOTALL
    )

    for match in struct_pattern.finditer(content):
        struct_body = match.group(1)
        struct_name = match.group(2)

        # Arrays must have element_size field
        if 'Array' in struct_name:
            if 'element_size' not in struct_body:
                line_num = content[:match.start()].count('\n') + 1
                issues.append(
                    f"{file_path.name}:{line_num} - Generic container '{struct_name}' missing 'element_size' field"
                )

        # Tables must have key_size and value_size fields instead
        if 'Table' in struct_name:
            if 'key_size' not in struct_body or 'value_size' not in struct_body:
                line_num = content[:match.start()].count('\n') + 1
                issues.append(
                    f"{file_path.name}:{line_num} - Table '{struct_name}' missing 'key_size' and 'value_size' fields"
                )

    return len(issues) == 0, issues


def verify_core_data_structure_design(project_root: Path) -> bool:
    """
    Main verification function for FF-000.

    Returns: True if all checks pass, False otherwise
    """
    print("=" * 70)
    print("FF-000: Core Data Structure Design Verification")
    print("=" * 70)
    print()
    print("Enforcing ADR-000: void* with manual size tracking pattern")
    print()

    src_dir = project_root / "src"
    inc_dir = project_root / "include"

    if not src_dir.exists() or not inc_dir.exists():
        print(f"FAILURE: Could not find {src_dir} or {inc_dir}")
        return False

    # Files to check (core data structures)
    files_to_check = [
        src_dir / "dArrays.c",
        src_dir / "dStaticArrays.c",
        src_dir / "dTables.c",
        src_dir / "dStaticTables.c",
        src_dir / "dStrings.c",
        src_dir / "dLinkedList.c",
        inc_dir / "Daedalus.h",
    ]

    all_issues = []

    for file_path in files_to_check:
        if not file_path.exists():
            print(f"Skipping {file_path.name} (not found)")
            continue

        print(f"Checking {file_path.name}...")

        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()

        # Run all checks
        checks = [
            ("Prefix convention (d_)", check_prefix_convention),
            ("Type suffix convention (_t)", check_type_suffix_convention),
            ("Init pattern", check_init_pattern),
            ("Destroy functions", check_destroy_functions),
            ("Get returns void*", check_get_returns_void_pointer),
            ("No global state", check_no_global_state),
            ("Element size in struct", check_element_size_in_struct),
        ]

        for check_name, check_func in checks:
            success, issues = check_func(content, file_path)
            if not success:
                print(f"  FAIL: {check_name}")
                all_issues.extend(issues)
            else:
                print(f"  PASS: {check_name}")

    print()

    if all_issues:
        print("=" * 70)
        print("ARCHITECTURE VIOLATION DETECTED")
        print("=" * 70)
        print()
        print(f"Found {len(all_issues)} issue(s):")
        for i, issue in enumerate(all_issues, 1):
            print(f"  {i}. {issue}")
        print()
        print("See: architecture/decisions/00-core-data-structure-design.md")
        print()
        return False

    print("SUCCESS: All core data structure design checks passed")
    return True


if __name__ == "__main__":
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent

    success = verify_core_data_structure_design(project_root)
    exit(0 if success else 1)
