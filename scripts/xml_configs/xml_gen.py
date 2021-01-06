import xml.etree.ElementTree as ET

detector_list = [
    ['0',9,0],
    ['1',9,1],
    ['2',9,2],
    ['3',9,3],
    ['4',9,4],
    ['5',9,5],
    ['6',9,6],
    ['7',9,7]
]

hagrid_list = [
    ['0',7,1],
    ['1',7,3],
    ['2',7,5],
    ['3',7,7],
    ['4',7,9],
    ['5',7,11],
    ['6',7,13],
    ['7',7,15],
    ['8',8,1],
    ['9',8,15],
    ['10',8,3],
    ['11',8,13]
]

hagridan_list = [
    ['0',7,0],
    ['1',7,2],
    ['2',7,4],
    ['3',7,6],
    ['4',7,8],
    ['5',7,10],
    ['6',7,12],
    ['7',7,14],
    ['8',8,0],
    ['9',8,14],
    ['10',8,2],
    ['11',8,12]
]

param_dict = {}
param_hag_dict = {}
param_hagan_dict = {}

def indent(elem, level=0):
    i = "\n" + level*"  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "\t"
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            indent(elem, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

def generateXML(filename="filename",run=0):
    # substitutes <Calibration> parameters of the xml file named [filename]
    # with param_dict[ch][run] and writes a file named Config-[run].xml
    xml_tree = ET.parse(filename)
    xml_root = xml_tree.getroot()
    for det in detector_list:
        det_params = param_dict[det[0]]
        params = det_params[str(run)]
        xml_module = xml_root.find("./Map/Module[@number='"+str(det[1])+"']")
        xml_channel = xml_module.find("./Channel[@number='"+str(det[2])+"']")
        xml_calib = xml_channel.find("./Calibration")
        calib_text = ''
        for param in params:
            calib_text = calib_text + ' ' + str(param)
        xml_calib.text = calib_text
	if det[0] == '0' or det[0] == '3':
	    xml_calib.set('model','quadratic')
	else:
	    xml_calib.set('model','linear')

        
    for det in hagrid_list:
        det_params = param_hag_dict[det[0]]
        params = det_params[str(run)]
        xml_module = xml_root.find("./Map/Module[@number='"+str(det[1])+"']")
        xml_channel = xml_module.find("./Channel[@number='"+str(det[2])+"']")
        xml_calib = xml_channel.find("./Calibration")
        calib_text = ''
        for param in params:
            calib_text = calib_text + ' ' + str(param)
        xml_calib.text = calib_text
	xml_calib.set('model','quadratic')

    for det in hagridan_list:
        det_params = param_hagan_dict[det[0]]
        params = det_params[str(run)]
        xml_module = xml_root.find("./Map/Module[@number='"+str(det[1])+"']")
        xml_channel = xml_module.find("./Channel[@number='"+str(det[2])+"']")
        xml_calib = xml_channel.find("./Calibration")
        calib_text = ''
        for param in params:
            calib_text = calib_text + ' ' + str(param)
        xml_calib.text = calib_text

    # Add an element 'Walk' to each vandle channel.
    xml_modules = xml_root.findall("./Map/Module")
    for module in xml_modules:
        xml_channels = module.findall("./Channel[@type='vandle']")
        if xml_channels == None:
            continue
        for channel in xml_channels:
            a = ET.Element('Walk')
            a.attrib['model'] = 'VM'
            a.text = " "
            channel.append(a)
            indent(channel,3)

    # Write modified xml to file.
    xml_tree.write('Config-production_'+str(run)+'.xml')

if __name__ == '__main__':
    for det in detector_list:
        f = open('det'+str(det[0])+'_data_fitresults.txt','r')
        # opens the input text file with 'run' 'param0' 'param1' ... format
        lines = [s.split() for s in f.readlines()]
        params = {s[0]:[float(s2) for s2 in s[1:]] for s in lines}
        param_dict.update({det[0]:params})
        f.close()

    for det in hagrid_list:
        f = open('hagdyn'+str(det[0])+'.txt','r')
        # opens the input text file with 'run' 'param0' 'param1' ... format
        lines = [s.split() for s in f.readlines()]
        params = {s[0]:[float(s2) for s2 in s[1:]] for s in lines}
        param_hag_dict.update({det[0]:params})
        f.close()

    for det in hagridan_list:
        f = open('hagan'+str(det[0])+'.txt','r')
        # opens the input text file with 'run' 'param0' 'param1' ... format
        lines = [s.split() for s in f.readlines()]
        params = {s[0]:[float(s2) for s2 in s[1:]] for s in lines}
        param_hagan_dict.update({det[0]:params})
        f.close()

    f = open('hagrid.txt')
    lines = [s.split() for s in f.readlines()]
    for line in lines:
        params = {}
        for i in range(14,113):
            params.update({str(i):[float(s2) for s2 in line[1:]]})
            param_hag_dict.update({line[0]:params})
    f.close()

    for i in range(14,68):
        if i == 27:
            continue
        generateXML(filename="ConfigProduction14to66.xml",run=i)
    for i in range(68,105):
        if i == 76:
            continue
        generateXML(filename="ConfigProduction68to103.xml",run=i)
    for i in range(105,113):
        generateXML(filename="ConfigProduction105to112.xml",run=i)
