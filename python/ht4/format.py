import sys
import argparse


def create_paragraph(input_):
    lines = input_.read()
    lines = lines.split('\n')
    current_paragraph = []
    paragraphs = []
    for line in lines:
        if line == '' and current_paragraph == []:
            pass
        else:
            if line == '':
                paragraphs.append(''.join(current_paragraph))
                current_paragraph = []
            else:
                current_paragraph.append(line)
                current_paragraph.append(' ')
    if current_paragraph:
        paragraphs.append(''.join(current_paragraph))
    return paragraphs


def create_words(string):
    punctuation_marks = ['.', ',', ':', '!', '?', '-', '\'']
    words = []
    current_word = []
    space = False
    for char in string:
        if char == ' ':
            space = True
        elif space and char not in punctuation_marks:
            words.append(''.join(current_word))
            current_word = [char]
            space = False
        else:
            current_word.append(char)
    if current_word:
        words.append(''.join(current_word))
    return words


def make_paragraph(words, line_length, paragraph_spaces):
    paragraph = []
    current_line = ''
    words[0] = ' ' * paragraph_spaces + words[0]
    for word in words:
        if len(word) + len(current_line) <= line_length:
            current_line += word + ' '
        elif len(word) <= line_length:
            paragraph.append(current_line.rstrip(' '))
            current_line = word + ' '
        else:
            print 'TROLOLO'
            exit(-1)
    if current_line:
        paragraph.append(current_line.rstrip(' '))
    return paragraph


def print_text(correct_text, output_):
    if output_ == sys.stdout:
        output_.write('\n')
        for it in correct_text:
            output_.write('\n'.join(it))
            output_.write('\n')
    else:
        output_file = open(output_.name, "w")
        for it in correct_text:
            output_file.write('\n'.join(it))
            output_file.write('\n')
        output_file.close()


def reformat(input_, output_,
             line_length, paragraph_spaces):
    paragraphs = create_paragraph(input_)
    correct_paragraph = []
    for paragraph in paragraphs:
        words = create_words(paragraph)
        correct_paragraph.append(make_paragraph(
            words, line_length, paragraph_spaces))
    print_text(correct_paragraph, output_)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-i', '--input',
        type=file, default=sys.stdin,
        help='from where data will be read')
    parser.add_argument(
        '-o', '--output',
        type=file, default=sys.stdout,
        help='where data will be stored')
    parser.add_argument(
        '-l', '--line-length',
        type=int, default=None,
        required=True,
        help='max length of string')
    parser.add_argument(
        '-p', '--paragraph-spaces',
        type=int, default=None,
        required=True,
        help='len of indent')
    args = parser.parse_args()
    reformat(args.input, args.output,
             args.line_length, args.paragraph_spaces)


if __name__ == '__main__':
    main()
