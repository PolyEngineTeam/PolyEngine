

def first_tagged_line_from_text(text, tag):
    tag_idx = text.find(tag)
    if tag_idx < 0:
        return None
    line_start = tag_idx + len(tag)
    line_end = text.find('\n', tag_idx)
    if line_end < 0:
        line_end = len(text)-1
    return text[line_start : line_end].lstrip()