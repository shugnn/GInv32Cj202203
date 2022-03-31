# -*- coding: utf-8 -*-
from PIL import Image,ImageGrab
from numpy import array
import os,sys#,time,mmap,json
def SpliteImg(imgarr):
    charac=[]
    pix=[]
    jj = 0
    maxwidth=0
    beghight=len(imgarr)
    endhight=0
    for j in range(len(imgarr[0])):
        blank=True
        for i in range(len(imgarr)):
            if imgarr[i,j]==0:
                pix.append((i,jj))
                blank=False
                beghight=min(i,beghight)
                endhight=max(i,endhight)
        if blank:
            if jj>0:charac.append(pix)
            pix=[]
            jj=0
        else:
            jj+=1
            maxwidth=max(jj,maxwidth)
    imgsp=[]
    for cha in charac:
        imgmake = array(Image.new('L', (15, 21)).point(lambda x: 255))
        for k in cha:
            #imgmake[k[0] - beghight + 2, k[1] + 2] = 0
            if k[0] < 21 and k[1] + 2 < 15:
                imgmake[k[0], k[1] + 2] = 0
            else:print('Larger ',k)
        imgsp.append(imgmake)
    return imgsp
def GetSingleImg(box):
    backgrey = ImageGrab.grab(box).convert('L')
    back = 0
    for j in range(2):
        for i in range(len(array(backgrey))):
            back += array(backgrey)[i, j]
    back = int(back / (2 * len(array(backgrey)) * 1.2))
    imgarr = array(backgrey.point(lambda x: 255 if x > back else 0))
    imgsp=SpliteImg(imgarr)
    return imgsp
def LoadTrainSet():
    #trainSetDir = os.getcwd()[:2] + '\\work\\samples_cjsc_hz_V103'
    trainSetDir = 'samples_cjsc_hz_V103'
    trainingSet = []
    for root,dirs,files in os.walk(trainSetDir):
        for dir in dirs:
            if not dir.isdigit():continue
            for root2,dirs2,files2 in os.walk(os.path.join(root,dir)):
                for file in files2:
                    fullname=os.path.join(root2,file)
                    imgarr = array(Image.open(fullname).convert('L').point(lambda x: 255 if x > 136 else 0))
                    trainingSet.append([dir, imgarr])
    return trainingSet
def Distance(sourceImg,trainingImg):
    distance=0
    for i in range(len(sourceImg)):
        for j in range(len(sourceImg[0])):
            distance+=((int(sourceImg[i,j])-int(trainingImg[i,j]))**2)
    return distance
def Predict(box):
    trainingSet=LoadTrainSet()
    imgsp=GetSingleImg(box)
    result=[]
    for sourceImg in imgsp:
        res = 'n'
        mindis = sys.maxsize
        for name,imgarr in trainingSet:
            distance=Distance(sourceImg,imgarr)
            if distance<mindis:
                mindis=distance
                res=name
        result.append(res)
    return result
if __name__ == '__main__':
    rst='rstNull'
    argc=len(sys.argv)
    if argc==5:
        box=tuple((int(sys.argv[1]),int(sys.argv[2]),int(sys.argv[3]),int(sys.argv[4])))
        rst = ''.join(Predict(box))
    else:
        rst='argcErr'
    print(rst)
