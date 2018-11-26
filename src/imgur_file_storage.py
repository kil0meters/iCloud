#!/usr/bin/python3
# import tar
from argparse import ArgumentParser

import png_encoder

def main():
    parser = ArgumentParser(prog='imgur_file_storage', description='Imgur cloud storage')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-e', '--encode', help='input file to encode',
                        type=str)
    group.add_argument('-d', '--decode', help='input file to decode',
                        type=str)
    parser.add_argument('-o', '--output', help='output file',
                        required=True)
    args = parser.parse_args()

    if not args.encode == None:
        png_encoder.encode_png(args.encode, args.output)
    else:
        png_encoder.decode_png(args.decode)
        # print(data.decode())

if __name__ == '__main__':
    main()
