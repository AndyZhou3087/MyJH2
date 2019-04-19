<GameFile>
  <PropertyGroup Name="popNewHeroLayer" Type="Layer" ID="8cea7298-a6c2-455b-af0e-3636272cbc69" Version="3.10.0.0" />
  <Content ctype="GameProjectContent">
    <Content>
      <Animation Duration="84" Speed="1.0000">
        <Timeline ActionTag="-2010061543" Property="FileData">
          <TextureFrame FrameIndex="0" Tween="False">
            <TextureFile Type="Normal" Path="images/newheroanim/01.png" Plist="" />
          </TextureFrame>
          <TextureFrame FrameIndex="7" Tween="False">
            <TextureFile Type="Normal" Path="images/newheroanim/02.png" Plist="" />
          </TextureFrame>
          <TextureFrame FrameIndex="14" Tween="False">
            <TextureFile Type="Normal" Path="images/newheroanim/03.png" Plist="" />
          </TextureFrame>
          <TextureFrame FrameIndex="21" Tween="False">
            <TextureFile Type="Normal" Path="images/newheroanim/04.png" Plist="" />
          </TextureFrame>
          <TextureFrame FrameIndex="28" Tween="False">
            <TextureFile Type="Normal" Path="images/newheroanim/05.png" Plist="" />
          </TextureFrame>
          <TextureFrame FrameIndex="35" Tween="False">
            <TextureFile Type="Normal" Path="images/newheroanim/06.png" Plist="" />
          </TextureFrame>
          <TextureFrame FrameIndex="42" Tween="False">
            <TextureFile Type="Normal" Path="images/newheroanim/07.png" Plist="" />
          </TextureFrame>
          <TextureFrame FrameIndex="49" Tween="False">
            <TextureFile Type="Normal" Path="images/newheroanim/08.png" Plist="" />
          </TextureFrame>
          <TextureFrame FrameIndex="56" Tween="False">
            <TextureFile Type="Normal" Path="images/newheroanim/09.png" Plist="" />
          </TextureFrame>
          <TextureFrame FrameIndex="63" Tween="False">
            <TextureFile Type="Normal" Path="images/newheroanim/10.png" Plist="" />
          </TextureFrame>
          <TextureFrame FrameIndex="70" Tween="False">
            <TextureFile Type="Normal" Path="images/newheroanim/11.png" Plist="" />
          </TextureFrame>
          <TextureFrame FrameIndex="77" Tween="False">
            <TextureFile Type="Normal" Path="images/newheroanim/12.png" Plist="" />
          </TextureFrame>
          <TextureFrame FrameIndex="84" Tween="False">
            <TextureFile Type="Normal" Path="images/newheroanim/13.png" Plist="" />
          </TextureFrame>
        </Timeline>
      </Animation>
      <ObjectData Name="Layer" Tag="32" ctype="GameLayerObjectData">
        <Size X="720.0000" Y="1280.0000" />
        <Children>
          <AbstractNodeData Name="animnode" ActionTag="56606202" Tag="8" IconVisible="True" LeftMargin="360.0000" RightMargin="360.0000" TopMargin="540.0000" BottomMargin="740.0000" ctype="SingleNodeObjectData">
            <Size X="0.0000" Y="0.0000" />
            <Children>
              <AbstractNodeData Name="effect" ActionTag="-2010061543" Tag="10" IconVisible="False" LeftMargin="-386.0000" RightMargin="-386.0000" TopMargin="-386.0000" BottomMargin="-386.0000" ctype="SpriteObjectData">
                <Size X="772.0000" Y="772.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="Normal" Path="images/newheroanim/13.png" Plist="" />
                <BlendFunc Src="1" Dst="771" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint />
            <Position X="360.0000" Y="740.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="0.5781" />
            <PreSize X="0.0000" Y="0.0000" />
          </AbstractNodeData>
          <AbstractNodeData Name="heronode" ActionTag="-873378487" Tag="7" IconVisible="True" LeftMargin="360.0000" RightMargin="360.0000" TopMargin="540.0000" BottomMargin="740.0000" ctype="SingleNodeObjectData">
            <Size X="0.0000" Y="0.0000" />
            <Children>
              <AbstractNodeData Name="light" ActionTag="1419321373" Tag="184" IconVisible="False" LeftMargin="-340.9060" RightMargin="-344.0940" TopMargin="-341.6329" BottomMargin="-343.3671" ctype="SpriteObjectData">
                <Size X="685.0000" Y="685.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="1.5940" Y="-0.8671" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="Normal" Path="images/newherolight.png" Plist="" />
                <BlendFunc Src="1" Dst="771" />
              </AbstractNodeData>
              <AbstractNodeData Name="heroimg" ActionTag="-441386843" Tag="48" IconVisible="False" LeftMargin="-301.4088" RightMargin="-304.5912" TopMargin="-526.7912" BottomMargin="-599.2088" LeftEage="199" RightEage="199" TopEage="371" BottomEage="371" Scale9OriginX="199" Scale9OriginY="371" Scale9Width="208" Scale9Height="384" ctype="ImageViewObjectData">
                <Size X="606.0000" Y="1126.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="1.5912" Y="-36.2088" />
                <Scale ScaleX="0.5000" ScaleY="0.5000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="Normal" Path="images/hfull_0_0.png" Plist="" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint />
            <Position X="360.0000" Y="740.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="0.5781" />
            <PreSize X="0.0000" Y="0.0000" />
          </AbstractNodeData>
          <AbstractNodeData Name="textnode" ActionTag="1217906279" Tag="12" IconVisible="True" LeftMargin="360.0000" RightMargin="360.0000" TopMargin="540.0000" BottomMargin="740.0000" ctype="SingleNodeObjectData">
            <Size X="0.0000" Y="0.0000" />
            <Children>
              <AbstractNodeData Name="title" ActionTag="474044666" Tag="183" IconVisible="False" LeftMargin="-199.4060" RightMargin="-202.5940" TopMargin="-341.6367" BottomMargin="296.6367" LeftEage="132" RightEage="132" TopEage="14" BottomEage="14" Scale9OriginX="132" Scale9OriginY="14" Scale9Width="138" Scale9Height="17" ctype="ImageViewObjectData">
                <Size X="402.0000" Y="45.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="1.5940" Y="319.1367" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="MarkedSubImage" Path="text_zh_cn/newherotitle.png" Plist="text_zh_cn.plist" />
              </AbstractNodeData>
              <AbstractNodeData Name="heroqu" ActionTag="318760725" Tag="132" IconVisible="False" LeftMargin="-251.8002" RightMargin="102.8002" TopMargin="-238.9017" BottomMargin="142.9017" LeftEage="49" RightEage="49" TopEage="31" BottomEage="31" Scale9OriginX="49" Scale9OriginY="31" Scale9Width="51" Scale9Height="34" ctype="ImageViewObjectData">
                <Size X="149.0000" Y="96.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="-177.3002" Y="190.9017" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="MarkedSubImage" Path="text_zh_cn/heroattrqu_0.png" Plist="text_zh_cn.plist" />
              </AbstractNodeData>
              <AbstractNodeData Name="herovoc" ActionTag="1865509208" Tag="226" IconVisible="False" LeftMargin="140.0468" RightMargin="-219.0468" TopMargin="153.6852" BottomMargin="-304.6852" LeftEage="26" RightEage="26" TopEage="31" BottomEage="31" Scale9OriginX="26" Scale9OriginY="31" Scale9Width="27" Scale9Height="89" ctype="ImageViewObjectData">
                <Size X="79.0000" Y="151.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="179.5468" Y="-229.1852" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="MarkedSubImage" Path="text_zh_cn/newherovoc0_text.png" Plist="text_zh_cn.plist" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint />
            <Position X="360.0000" Y="740.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="0.5781" />
            <PreSize X="0.0000" Y="0.0000" />
          </AbstractNodeData>
        </Children>
      </ObjectData>
    </Content>
  </Content>
</GameFile>