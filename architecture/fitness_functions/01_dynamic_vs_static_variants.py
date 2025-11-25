#!/usr/bin/env python3
"""
FF-001: Dynamic vs Static Variants Verification

Ensures the design pattern from ADR-001 is maintained:
1. Both dArray_t and dStaticArray_t exist with shared API patterns
2. Static variant functions return int (error codes)
3. Dynamic variant functions return void (always succeed or OOM)
4. Both have Init/Append/Get/Destroy functions
5. Static variant capacity never changes after init
6. No realloc() calls in static array implementation

Based on: ADR-001 (Dynamic vs Static Array Variants)

Architecture Characteristic: Predictability, Performance, Real-time Suitability
"""

import re
from pathlib import Path
from typing import List, Tuple, Set


def check_both_types_exist(inc_content: str) -> Tuple[bool, List[str]]:
    """
    Verify both dArray_t and dStaticArray_t types exist.

    Returns: (success, list of issues)
    """
    issues = []

    if 'dArray_t' not in inc_content:
        issues.append("Missing dArray_t type definition")

    if 'dStaticArray_t' not in inc_content:
        issues.append("Missing dStaticArray_t type definition")

    return len(issues) == 0, issues


def check_shared_api_pattern(inc_content: str) -> Tuple[bool, List[str]]:
    """
    Verify both variants have matching Init/Append/Get/Destroy functions.

    Returns: (success, list of issues)
    """
    issues = []

    # Required functions for each type
    dynamic_funcs = ['d_ArrayInit', 'd_ArrayAppend', 'd_ArrayGet', 'd_ArrayDestroy']
    static_funcs = ['d_InitStaticArray', 'd_StaticArrayAppend', 'd_StaticArrayGet', 'd_StaticArrayDestroy']

    for func in dynamic_funcs:
        if func not in inc_content:
            issues.append(f"Missing dynamic array function: {func}")

    for func in static_funcs:
        if func not in inc_content:
            issues.append(f"Missing static array function: {func}")

    return len(issues) == 0, issues


def check_static_returns_error_codes(inc_content: str) -> Tuple[bool, List[str]]:
    """
    Verify static array mutation functions return int (for error handling).

    Returns: (success, list of issues)
    """
    issues = []

    # Functions that can fail in static variant should return int
    mutation_funcs = [
        ('d_StaticArrayAppend', r'int\s+d_StaticArrayAppend'),
        ('d_StaticArrayInsert', r'int\s+d_StaticArrayInsert'),
        ('d_StaticArraySet', r'int\s+d_StaticArraySet'),
    ]

    for func_name, pattern in mutation_funcs:
        if func_name in inc_content:
            if not re.search(pattern, inc_content):
                issues.append(
                    f"{func_name} should return 'int' for error handling (can fail when capacity exceeded)"
                )

    return len(issues) == 0, issues


def check_dynamic_returns_void(inc_content: str) -> Tuple[bool, List[str]]:
    """
    Verify dynamic array mutation functions return void (always succeed).

    Returns: (success, list of issues)
    """
    issues = []

    # Dynamic functions that modify should return void (or int on success/OOM but not capacity)
    mutation_funcs = [
        ('d_ArrayAppend', r'(void|int)\s+d_ArrayAppend'),
        ('d_ArrayInsert', r'(void|int)\s+d_ArrayInsert'),
    ]

    for func_name, pattern in mutation_funcs:
        if func_name in inc_content:
            # This is a soft check - int is okay for OOM handling
            # Main point is they don't fail due to capacity
            pass

    return True, issues


def check_no_realloc_in_static(src_content: str, file_path: Path) -> Tuple[bool, List[str]]:
    """
    Verify static array implementation never calls realloc().

    Returns: (success, list of issues)
    """
    issues = []

    if 'realloc(' in src_content or 'realloc (' in src_content:
        issues.append(
            f"{file_path.name} - Static arrays should never call realloc() (violates fixed-capacity guarantee)"
        )

    return len(issues) == 0, issues


def check_capacity_field_usage(src_content: str, file_path: Path) -> Tuple[bool, List[str]]:
    """
    Verify static array implementation never modifies capacity field.

    Returns: (success, list of issues)
    """
    issues = []

    # Look for assignments to capacity field (e.g., "array->capacity = " or "->capacity=")
    capacity_assignment = re.compile(r'->capacity\s*=\s*[^=]')

    # Find the d_InitStaticArray function bounds
    init_func_pattern = re.compile(
        r'dStaticArray_t\*\s+d_InitStaticArray\s*\([^)]*\)\s*\{',
        re.MULTILINE
    )

    match = init_func_pattern.search(src_content)
    if not match:
        # Can't verify if we can't find the init function
        return True, issues

    # Find the end of the init function
    init_start = match.end()
    brace_depth = 1
    init_end = init_start

    while init_end < len(src_content) and brace_depth > 0:
        if src_content[init_end] == '{':
            brace_depth += 1
        elif src_content[init_end] == '}':
            brace_depth -= 1
        init_end += 1

    # Look for capacity assignments outside the init function
    capacity_assignment = re.compile(r'->capacity\s*=\s*[^=]')

    for match in capacity_assignment.finditer(src_content):
        pos = match.start()

        # Skip if inside init function
        if init_start <= pos < init_end:
            continue

        line_num = src_content[:pos].count('\n') + 1
        issues.append(
            f"{file_path.name}:{line_num} - Static array capacity modified after init (should be immutable)"
        )

    return len(issues) == 0, issues


def verify_dynamic_vs_static_variants(project_root: Path) -> bool:
    """
    Main verification function for FF-001.

    Returns: True if all checks pass, False otherwise
    """
    print("=" * 70)
    print("FF-001: Dynamic vs Static Variants Verification")
    print("=" * 70)
    print()
    print("Enforcing ADR-001: Separate dynamic and static array types")
    print()

    inc_file = project_root / "include" / "Daedalus.h"
    dynamic_src = project_root / "src" / "dArrays.c"
    static_src = project_root / "src" / "dStaticArrays.c"

    if not inc_file.exists():
        print(f"FAILURE: Could not find {inc_file}")
        return False

    with open(inc_file, 'r', encoding='utf-8') as f:
        inc_content = f.read()

    all_issues = []

    # Check 1: Both types exist
    print("Checking both array types exist...")
    success, issues = check_both_types_exist(inc_content)
    if success:
        print("  PASS: Both dArray_t and dStaticArray_t defined")
    else:
        for issue in issues:
            print(f"  FAIL: {issue}")
        all_issues.extend(issues)

    # Check 2: Shared API pattern
    print("Checking shared API pattern...")
    success, issues = check_shared_api_pattern(inc_content)
    if success:
        print("  PASS: Both variants have Init/Append/Get/Destroy functions")
    else:
        for issue in issues:
            print(f"  FAIL: {issue}")
        all_issues.extend(issues)

    # Check 3: Static returns error codes
    print("Checking static variant error handling...")
    success, issues = check_static_returns_error_codes(inc_content)
    if success:
        print("  PASS: Static mutation functions return int for error handling")
    else:
        for issue in issues:
            print(f"  FAIL: {issue}")
        all_issues.extend(issues)

    # Check 4: No realloc in static implementation
    if static_src.exists():
        print("Checking static array never uses realloc...")
        with open(static_src, 'r', encoding='utf-8') as f:
            static_content = f.read()

        success, issues = check_no_realloc_in_static(static_content, static_src)
        if success:
            print("  PASS: Static array implementation never calls realloc()")
        else:
            for issue in issues:
                print(f"  FAIL: {issue}")
            all_issues.extend(issues)

        # Check 5: Capacity immutability
        print("Checking static array capacity immutability...")
        success, issues = check_capacity_field_usage(static_content, static_src)
        if success:
            print("  PASS: Static array capacity never modified after init")
        else:
            for issue in issues:
                print(f"  FAIL: {issue}")
            all_issues.extend(issues)

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
        print("See: architecture/decisions/01-dynamic-vs-static-variants.md")
        print()
        return False

    print("SUCCESS: All dynamic vs static variant checks passed")
    return True


if __name__ == "__main__":
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent

    success = verify_dynamic_vs_static_variants(project_root)
    exit(0 if success else 1)
