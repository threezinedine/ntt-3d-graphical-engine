import os
import argparse


def main():
    parser = argparse.ArgumentParser(description="Run the embedded script.")

    parser.add_argument(
        "input_pair",
        type=str,
        help="Input file path and variable name pair in the format 'input_file::variable_name'",
    )

    parser.add_argument(
        "output",
        type=str,
        default="output.txt",
        help="Output file path",
    )

    args = parser.parse_args()

    input_pair = args.input_pair.split("::")
    assert (
        len(input_pair) == 2
    ), "Input pair must be in the format 'input_file::variable_name'"
    input_file, variable_name = input_pair

    if not os.path.exists(input_file):
        print(f"Input file '{input_file}' does not exist.")
        return

    with open(input_file, "rb") as file:
        content = file.read()

    with open(args.output, "w") as file:
        output_content = ""
        for index, byte in enumerate(content):
            output_content += f"0x{byte:02x}, "
            if (index + 1) % 12 == 0:
                output_content += "\n\t"
        file.write(f"unsigned char {variable_name}[] = {{\n\t{output_content}\n}};\n")


if __name__ == "__main__":
    main()
