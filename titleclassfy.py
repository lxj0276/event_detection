#!/usr/bin/env python
# coding:utf-8
import os,sys
from urllib import unquote
reload(sys) 
sys.setdefaultencoding('utf-8')
sys.path.append('../cls/')
from utils import str_to_utf8,str_to_unicode
sys.path.append('../cls/text_cls')
from text_cls import ws,text_classifier,predict_content 
# word_seg
ws = ws
# cls model
basepath = "/data/seanxywang/code/cls/text_cls/"
text_classifier.load_model(basepath + "model/") 
def TextClsHandler(str):
    res = predict_content(str,text_classifier)
    if res and res[0][1] > 0.05:
        s = ""
        for i in range(1):#range(len(res)):
            cid,score = res[i][0],res[i][1]
            return cid
    return -1;        
if __name__ == "__main__":
    fi = open(sys.argv[1],'r')
    fo = open(sys.argv[2],'a')
    for line in fi:
        scrstr = line.replace("\n",'').replace(" ",'')        
        cid = TextClsHandler(scrstr.split("\t")[4])
        if cid == '4':
            fo.write(scrstr + "\n")
  


