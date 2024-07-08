import sys

def main():
    if len(sys.argv) > 1:
        for i, arg in enumerate(sys.argv[1:], start=1):
            print(f"Argument {i}: {arg}")
    else:
        print("No arguments provided.")

if __name__ == "__main__":
    main()
