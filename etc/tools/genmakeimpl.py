#!/usr/bin/env python3

import argparse
import os.path

from mako.template import Template

template_dir = os.path.dirname(__file__)
template_filename = os.path.join(template_dir, 'makeimpl.hpp.mako')

def generate(arg_count: int, output_path: str):
    output_dir = os.path.join(os.path.dirname(__file__), output_path)
    output_filename = os.path.join(output_dir, 'generated_makeimpl.hpp')
    
    with open(output_filename, 'w') as f:
        template = Template(filename=template_filename)
        f.write(template.render(arg_count=arg_count))
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate `Addle::Config::detail::Injector::make_impl` declarations and definitions.')
    parser.add_argument('--arg-count', dest='arg_count', type=int, default=16)
    parser.add_argument('Output', nargs='?', default='../../src/common/utilities/config')
    args = parser.parse_args()
    
    generate(args.arg_count, args.Output)
