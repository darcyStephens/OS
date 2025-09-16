from clockmmu import ClockMMU
from lrummu import LruMMU
from randmmu import RandMMU
import logging
import sys
import yaml
import json
import os
import time
from pathlib import Path
import matplotlib.pyplot as plt

logging.basicConfig(level=logging.WARNING, format="%(levelname)s: %(message)s")


def run_single_simulation(input_file, frames, replacement_mode):
    """Run a single simulation and return results"""
    PAGE_OFFSET = 12  # page is 2^12 = 4KB

    # Setup MMU based on replacement mode
    if replacement_mode == "rand":
        mmu = RandMMU(frames)
    elif replacement_mode == "lru":
        mmu = LruMMU(frames)
    elif replacement_mode == "clock":
        mmu = ClockMMU(frames)
    else:
        return None

    mmu.reset_debug()  # Always quiet mode for experiments

    no_events = 0
    start_time = time.time()  # Start timing

    # Process trace file
    with open(input_file, "r") as trace_file:
        for trace_line in trace_file:
            trace_cmd = trace_line.strip().split(" ")
            logical_address = int(trace_cmd[0], 16)
            page_number = logical_address >> PAGE_OFFSET

            # Process read or write
            if trace_cmd[1] == "R":
                mmu.read_memory(page_number)
            elif trace_cmd[1] == "W":
                mmu.write_memory(page_number)
            else:
                return None

            no_events += 1

    end_time = time.time()  # End timing
    execution_time = end_time - start_time

    # Calculate hit rate (1 - fault rate)
    fault_rate = mmu.get_total_page_faults() / no_events if no_events > 0 else 0
    hit_rate = 1.0 - fault_rate

    # Return results as dictionary
    return {
        "trace_file": input_file,
        "frames": frames,
        "algorithm": replacement_mode,
        "events": no_events,
        "disk_reads": mmu.get_total_disk_reads(),
        "disk_writes": mmu.get_total_disk_writes(),
        "page_faults": mmu.get_total_page_faults(),
        "fault_rate": fault_rate,
        "hit_rate": hit_rate,
        "execution_time": execution_time,
    }


def run_experiments(config_file):
    """Run all experiments based on YAML config"""

    # Load configuration
    with open(config_file, "r") as f:
        config = yaml.safe_load(f)

    # Handle both old format (single experiment) and new format (list of experiments)
    if "experiments" in config and isinstance(config["experiments"], list):
        experiments_list = config["experiments"]
        output_config = config["output"]
    else:
        # Old format - convert to new format
        experiments_list = [config["experiments"]]
        output_config = config["experiments"]["output"]

    all_results = []

    for exp_idx, experiment in enumerate(experiments_list):
        exp_name = experiment.get("name", f"experiment_{exp_idx + 1}")
        exp_description = experiment.get("description", "")

        print(f"\n{'=' * 60}")
        print(f"Running Experiment: {exp_name}")
        if exp_description:
            print(f"Description: {exp_description}")
        print(f"{'=' * 60}")

        # Generate frame sizes for this experiment
        if isinstance(experiment["frame_sizes"], dict):
            if "step" in experiment["frame_sizes"]:
                # Linear frame sizes with min/max/step
                frame_sizes = list(
                    range(
                        experiment["frame_sizes"]["min"],
                        experiment["frame_sizes"]["max"] + 1,
                        experiment["frame_sizes"]["step"],
                    )
                )
            else:
                # Exponential frame sizes (powers of 2)
                min_exp = experiment["frame_sizes"]["min"]
                max_exp = experiment["frame_sizes"]["max"]
                frame_sizes = [2**i for i in range(min_exp, max_exp + 1)]
        else:
            frame_sizes = experiment["frame_sizes"]

        total_experiments = (
            len(experiment["traces"]) * len(experiment["algorithms"]) * len(frame_sizes)
        )
        current_experiment = 0

        print(f"Running {total_experiments} simulations for this experiment...")

        # Run all combinations for this experiment
        for trace in experiment["traces"]:
            if not os.path.exists(trace):
                print(f"Warning: {trace} not found, skipping...")
                continue

            print(f"\nProcessing {trace}...")

            for algorithm in experiment["algorithms"]:
                print(f"  Algorithm: {algorithm}")

                for frames in frame_sizes:
                    current_experiment += 1
                    print(
                        f"    Frames: {frames} ({current_experiment}/{total_experiments})"
                    )

                    result = run_single_simulation(trace, frames, algorithm)

                    if result:
                        # Add experiment metadata to result
                        result["experiment_name"] = exp_name
                        result["experiment_description"] = exp_description
                        all_results.append(result)

    # Save results
    output_file = output_config["results_file"]
    with open(output_file, "w") as f:
        json.dump(all_results, f, indent=2)

    print(f"\nResults saved to {output_file}")

    # Generate graphs
    generate_graphs(all_results, output_config["graphs_dir"])

    # Print summary
    print_summary(all_results)

    return all_results


def generate_graphs(results, graphs_dir):
    """Generate graphs from experiment results"""
    os.makedirs(graphs_dir, exist_ok=True)

    # Group results by trace and experiment
    traces = {}
    for result in results:
        trace = result["trace_file"]
        exp_name = result.get("experiment_name", "default")

        # Create unique key for trace + experiment combination
        trace_key = f"{Path(trace).stem}_{exp_name}"

        if trace_key not in traces:
            traces[trace_key] = {"rand": [], "lru": [], "clock": []}
        traces[trace_key][result["algorithm"]].append(result)

    # Generate graphs for each trace/experiment combination
    for trace_key, trace_data in traces.items():
        # Page fault rate vs frames
        plt.figure(figsize=(12, 8))

        for algorithm, data in trace_data.items():
            if data:  # Only plot if we have data
                data_sorted = sorted(data, key=lambda x: x["frames"])
                frames = [d["frames"] for d in data_sorted]
                fault_rates = [d["fault_rate"] for d in data_sorted]

                plt.plot(
                    frames,
                    fault_rates,
                    marker="o",
                    label=algorithm.upper(),
                    linewidth=2,
                )

        plt.xlabel("Number of Frames")
        plt.ylabel("Page Fault Rate")
        plt.title(f"Page Fault Rate vs Frame Size - {trace_key}")
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.savefig(
            f"{graphs_dir}/{trace_key}_fault_rate.png", dpi=300, bbox_inches="tight"
        )
        plt.close()

        # Disk operations vs frames
        plt.figure(figsize=(12, 8))

        for algorithm, data in trace_data.items():
            if data:
                data_sorted = sorted(data, key=lambda x: x["frames"])
                frames = [d["frames"] for d in data_sorted]
                disk_ops = [d["disk_reads"] + d["disk_writes"] for d in data_sorted]

                plt.plot(
                    frames, disk_ops, marker="s", label=algorithm.upper(), linewidth=2
                )

        plt.xlabel("Number of Frames")
        plt.ylabel("Total Disk Operations")
        plt.title(f"Disk Operations vs Frame Size - {trace_key}")
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.savefig(
            f"{graphs_dir}/{trace_key}_disk_ops.png", dpi=300, bbox_inches="tight"
        )
        plt.close()

        # NEW: Execution time vs frames
        plt.figure(figsize=(12, 8))

        for algorithm, data in trace_data.items():
            if data:
                data_sorted = sorted(data, key=lambda x: x["frames"])
                frames = [d["frames"] for d in data_sorted]
                exec_times = [d["execution_time"] for d in data_sorted]

                plt.plot(
                    frames, exec_times, marker="^", label=algorithm.upper(), linewidth=2
                )

        plt.xlabel("Number of Frames")
        plt.ylabel("Execution Time (seconds)")
        plt.title(f"Execution Time vs Frame Size - {trace_key}")
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.savefig(
            f"{graphs_dir}/{trace_key}_execution_time.png",
            dpi=300,
            bbox_inches="tight",
        )
        plt.close()

        # NEW: Hit rate vs frames
        plt.figure(figsize=(12, 8))

        for algorithm, data in trace_data.items():
            if data:
                data_sorted = sorted(data, key=lambda x: x["frames"])
                frames = [d["frames"] for d in data_sorted]
                hit_rates = [d["hit_rate"] for d in data_sorted]

                plt.plot(
                    frames, hit_rates, marker="d", label=algorithm.upper(), linewidth=2
                )

        plt.xlabel("Number of Frames")
        plt.ylabel("Hit Rate (1 - Page Fault Rate)")
        plt.title(f"Hit Rate vs Frame Size - {trace_key}")
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.ylim(0, 1)  # Hit rate is between 0 and 1
        plt.savefig(
            f"{graphs_dir}/{trace_key}_hit_rate.png", dpi=300, bbox_inches="tight"
        )
        plt.close()

        # NEW: Hit rate vs execution time scatter plot
        plt.figure(figsize=(12, 8))

        for algorithm, data in trace_data.items():
            if data:
                hit_rates = [d["hit_rate"] for d in data]
                exec_times = [d["execution_time"] for d in data]

                plt.scatter(
                    hit_rates, exec_times, label=algorithm.upper(), alpha=0.7, s=50
                )

        plt.xlabel("Hit Rate (1 - Page Fault Rate)")
        plt.ylabel("Execution Time (seconds)")
        plt.title(f"Hit Rate vs Execution Time - {trace_key}")
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.xlim(0, 1)  # Hit rate is between 0 and 1
        plt.savefig(
            f"{graphs_dir}/{trace_key}_hitrate_vs_time.png",
            dpi=300,
            bbox_inches="tight",
        )
        plt.close()

        # NEW: Dual-axis plot - Hit rate and execution time vs frames
        fig, ax1 = plt.subplots(figsize=(12, 8))

        # Left y-axis for hit rate
        ax1.set_xlabel("Number of Frames")
        ax1.set_ylabel("Hit Rate (1 - Page Fault Rate)", color="blue")
        ax1.tick_params(axis="y", labelcolor="blue")
        ax1.set_ylim(0, 1)

        # Right y-axis for execution time
        ax2 = ax1.twinx()
        ax2.set_ylabel("Execution Time (seconds)", color="red")
        ax2.tick_params(axis="y", labelcolor="red")

        for algorithm, data in trace_data.items():
            if data:
                data_sorted = sorted(data, key=lambda x: x["frames"])
                frames = [d["frames"] for d in data_sorted]
                hit_rates = [d["hit_rate"] for d in data_sorted]
                exec_times = [d["execution_time"] for d in data_sorted]

                # Plot hit rate on left axis (blue tones)
                ax1.plot(
                    frames,
                    hit_rates,
                    marker="o",
                    linestyle="-",
                    label=f"{algorithm.upper()} Hit Rate",
                    linewidth=2,
                    alpha=0.8,
                )

                # Plot execution time on right axis (red tones)
                ax2.plot(
                    frames,
                    exec_times,
                    marker="s",
                    linestyle="--",
                    label=f"{algorithm.upper()} Time",
                    linewidth=2,
                    alpha=0.8,
                )

        # Combine legends from both axes
        lines1, labels1 = ax1.get_legend_handles_labels()
        lines2, labels2 = ax2.get_legend_handles_labels()
        ax1.legend(lines1 + lines2, labels1 + labels2, loc="center right")

        plt.title(f"Hit Rate & Execution Time vs Frame Size - {trace_key}")
        plt.grid(True, alpha=0.3)
        plt.savefig(
            f"{graphs_dir}/{trace_key}_dual_axis.png", dpi=300, bbox_inches="tight"
        )
        plt.close()

    print(f"Graphs saved to {graphs_dir}/")


def print_summary(results):
    """Print experiment summary"""
    print("\n=== EXPERIMENT SUMMARY ===")

    # Group by trace for analysis
    traces = {}
    for result in results:
        trace = result["trace_file"]
        if trace not in traces:
            traces[trace] = []
        traces[trace].append(result)

    for trace, trace_results in traces.items():
        trace_name = Path(trace).stem
        print(f"\n{trace_name}:")

        # Find performance at different memory levels
        algorithms = set(r["algorithm"] for r in trace_results)

        for memory_level in [50, 100, 200]:
            matching_results = [r for r in trace_results if r["frames"] == memory_level]
            if matching_results:
                print(f"  At {memory_level} frames:")

                for algorithm in algorithms:
                    alg_results = [
                        r for r in matching_results if r["algorithm"] == algorithm
                    ]
                    if alg_results:
                        result = alg_results[0]
                        print(
                            f"    {algorithm.upper()}: fault rate = {result['fault_rate']:.4f}, "
                            f"hit rate = {result['hit_rate']:.4f}, "
                            f"time = {result['execution_time']:.3f}s"
                        )


def main():
    if len(sys.argv) != 2:
        print("Usage: python memsim_experiments.py experiment_config.yaml")
        return

    config_file = sys.argv[1]

    if not os.path.exists(config_file):
        print(f"Config file {config_file} not found")
        return

    if not config_file.endswith(".yaml") and not config_file.endswith(".yml"):
        print("Config file must be a YAML file")
        return

    try:
        run_experiments(config_file)
        print("\nAll experiments completed successfully!")
    except Exception as e:
        print(f"Error running experiments: {e}")


if __name__ == "__main__":
    main()
