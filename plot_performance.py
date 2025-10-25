import subprocess
import re
import matplotlib.pyplot as plt

def run_test(array_size, cutoff, seed=1):
    """Run test-mergesort and return the time taken in seconds."""
    try:
        result = subprocess.run(
            ['./test-mergesort', str(array_size), str(cutoff), str(seed)],
            capture_output=True,
            text=True,
            timeout=60
        )
        match = re.search(r'took\s+([\d.]+)\s+seconds', result.stdout)
        if match:
            return float(match.group(1))
        else:
            print(f"Warning: Could not parse output for cutoff={cutoff}")
            return None
    except subprocess.TimeoutExpired:
        print(f"Warning: Test timed out for cutoff={cutoff}")
        return None
    except Exception as e:
        print(f"Error running test with cutoff={cutoff}: {e}")
        return None

def get_cutoffs_for_size(array_size):
    """Return appropriate cutoff values for a given array size."""
    if array_size <= 10:
        return [0, 2, 5, array_size]
    elif array_size <= 100:
        return [0, 2, 5, 10, 25, 50, array_size]
    elif array_size <= 1000:
        return [0, 5, 10, 25, 50, 100, 250, 500, array_size]
    else:
        return [0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 15, 20]

def test_array_size(array_size):
    """Test a single array size with various cutoff values."""
    cutoffs = [x for x in range(0, 13)]
    
    print(f"\n{'='*60}")
    print(f"Testing array size: {array_size}")
    print(f"Cutoff values: {cutoffs}")
    print("="*60)
    
    times = []
    valid_cutoffs = []
    
    for cutoff in cutoffs:
        print(f"  cutoff={cutoff:4d}...", end=" ")
        time_taken = run_test(array_size, cutoff)
        
        if time_taken is not None:
            times.append(time_taken)
            valid_cutoffs.append(cutoff)
            print(f"{time_taken:.4f} seconds")
        else:
            print("FAILED")
    
    return valid_cutoffs, times

def main():
    array_sizes = [10, 100, 1000,10000,100_000,1_000_000,100_000_000]
    
    print("Mergesort Performance Testing")
    print(f"Testing array sizes: {array_sizes}")
    
    all_results = {}
    
    for size in array_sizes:
        cutoffs, times = test_array_size(size)
        if times:
            all_results[size] = (cutoffs, times)
    
    if not all_results:
        print("\nError: No valid test results collected!")
        return
    
    for size, (cutoffs, times) in all_results.items():
        plt.figure(figsize=(10, 6))
        plt.plot(cutoffs, times, marker='o', linewidth=2, markersize=8, color='blue')
        plt.xlabel('Cutoff Value', fontsize=12)
        plt.ylabel('Time Taken (seconds)', fontsize=12)
        plt.title(f'Mergesort Performance: Array Size = {size}', fontsize=14, fontweight='bold')
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        
        filename = f'performance_cutoff_{size}.png'
        plt.savefig(filename, dpi=300)
        print(f"\nPlot saved: {filename}")
    
    plt.figure(figsize=(12, 7))
    for size, (cutoffs, times) in all_results.items():
        plt.plot(cutoffs, times, marker='o', linewidth=2, markersize=8, label=f'Size={size}')
    
    plt.xlabel('Cutoff Value', fontsize=12)
    plt.ylabel('Time Taken (seconds)', fontsize=12)
    plt.title('Mergesort Performance Comparison', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    
    combined_filename = 'performance_comparison.png'
    plt.savefig(combined_filename, dpi=300)
    print(f"Combined plot saved: {combined_filename}")
    
    print("\n" + "="*60)
    print("All tests completed!")
    print("="*60)
    
if __name__ == "__main__":
    main()
