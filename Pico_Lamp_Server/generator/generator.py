with open ('../data/light.html') as f:
    html_lines_light = [line.rstrip('\n') for line in f]

with open ('../data/light.css') as f:
    css_lines_light = [line.rstrip('\n') for line in f]

with open ('../data/dark.html') as f:
    html_lines_dark = [line.rstrip('\n') for line in f]

with open ('../data/dark.css') as f:
    css_lines_dark = [line.rstrip('\n') for line in f]

with open ('../template/template.cpp') as f:
    template_lines = [line.rstrip('\n') for line in f]

outfile = open("../src/main.cpp","w")

for values in template_lines:
    if "%HTML-DARK%" in values:
        for ln in html_lines_dark:
            outfile.write ('"'+ln.replace('"','\\"')+'"')
            outfile.write ('\n')

    elif "%HTML-LIGHT%" in values:
        for ln in html_lines_light:
            outfile.write ('"'+ln.replace('"','\\"')+'"')
            outfile.write ('\n')

    elif "%CSS-DARK%" in values:
        for ln in css_lines_dark:
            outfile.write ('"'+ln.replace('"','\\"')+'"')
            outfile.write ('\n')

    elif "%CSS-LIGHT%" in values:
        for ln in css_lines_light:
            outfile.write ('"'+ln.replace('"','\\"')+'"')
            outfile.write ('\n')

    else:
        outfile.write (values)
        outfile.write ('\n')