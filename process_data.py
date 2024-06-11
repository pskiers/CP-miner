import csv
import argparse


def parse_data(file: str, out_file: str, max_transactions: int | None = None, max_elements: int | None = None) -> None:
    rows = []
    elements: set[int] = set()

    with open(file, "r") as csvfile:
        csvreader = csv.reader(csvfile, delimiter=" ")
        for i, row in enumerate(csvreader):
            if max_transactions is not None and i >= max_transactions:
                break
            row_int = [int(item) for item in row if item != ""]
            if max_elements is not None and len(elements) < max_elements:
                elements.update(row_int)
            if max_elements is not None and len(elements) > max_elements:
                elements = set(list(elements)[:max_elements])
            if max_elements is not None:
                row_int = [element for element in row_int if element in elements]
            rows.append(row_int)
    with open(out_file, mode='w', newline='') as f:
        writer = csv.writer(f, delimiter=" ")
        writer.writerows(rows)    


def main():
    # parser = argparse.ArgumentParser()
    # parser.add_argument("-p", "--path", required=True, help="path to data file")
    # parser.add_argument("-o", "--out", required=True, help="output file path")
    # parser.add_argument("-t", "transaction", required=False, help="maximum number of transaction")
    # parser.add_argument("-e", "--elements", required=False, help="number of different elements")
    # args = parser.parse_args()
    # path = args.path
    # out = args.out
    # max_transactions = int(args.transaction) if args.transaction is not None else None
    # max_elements = int(args.elements) if args.elements is not None else None

    # parse_data(path, out, max_transactions, max_elements)
    for t in [50, 150, 250, 500]:
        for e in [None, 200, 300, 500, 1000]:
            parse_data("data/pumsb_star.dat", f"data/preprocessed/pumsb_star_t{t}_e{e}.dat", t, e)


if __name__ == "__main__":
    main()
