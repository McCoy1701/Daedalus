#!/usr/bin/env python3
"""
Fitness Function Runner

Executes all architecture fitness functions and reports results.
Exits with non-zero code if any fitness function fails.

Usage:
    python architecture/fitness_functions/main.py
    make verify
"""

import sys
from pathlib import Path
from typing import List, Tuple
import importlib.util


def load_fitness_function(name: str, file_path: Path):
    """Dynamically load a fitness function module"""
    spec = importlib.util.spec_from_file_location(name, file_path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def run_fitness_functions() -> List[Tuple[str, bool]]:
    """Run all fitness functions and collect results"""
    results = []

    print("=" * 70)
    print("ARCHITECTURE FITNESS FUNCTIONS - DAEDALUS")
    print("=" * 70)
    print()

    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent

    # FF-000: Core Data Structure Design
    try:
        ff000 = load_fitness_function(
            "core_data_structure_design",
            script_dir / "00_core_data_structure_design.py"
        )
        success = ff000.verify_core_data_structure_design(project_root)
        results.append(("FF-000: Core Data Structure Design", success))
    except Exception as e:
        print(f"FF-000 crashed: {e}")
        import traceback
        traceback.print_exc()
        results.append(("FF-000: Core Data Structure Design", False))

    print()

    # FF-001: Dynamic vs Static Array Variants
    try:
        ff001 = load_fitness_function(
            "dynamic_vs_static_variants",
            script_dir / "01_dynamic_vs_static_variants.py"
        )
        success = ff001.verify_dynamic_vs_static_variants(project_root)
        results.append(("FF-001: Dynamic vs Static Variants", success))
    except Exception as e:
        print(f"FF-001 crashed: {e}")
        import traceback
        traceback.print_exc()
        results.append(("FF-001: Dynamic vs Static Variants", False))

    print()

    # Add more fitness functions here as they're created:
    # FF-002: Hash Table Implementation
    # FF-003: DUF Format Design

    return results


def print_summary(results: List[Tuple[str, bool]]) -> bool:
    """Print summary and return overall pass/fail"""
    print("=" * 70)
    print("SUMMARY")
    print("=" * 70)
    print()

    passed = sum(1 for _, success in results if success)
    failed = len(results) - passed

    for name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{status}: {name}")

    print()
    print(f"Total: {passed}/{len(results)} passed, {failed}/{len(results)} failed")
    print()

    if failed == 0:
        print("All fitness functions passed!")
        return True
    else:
        print("Some fitness functions failed - see details above")
        return False


if __name__ == '__main__':
    results = run_fitness_functions()
    overall_success = print_summary(results)
    sys.exit(0 if overall_success else 1)
