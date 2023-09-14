//
//  DatePickerView.swift
//  AquariumControllerClient
//
//  Created by Connor Williams on 9/10/23.
//

import SwiftUI

struct DatePickerView: View {
    @Binding public var seconds: Int
    
    //var daysArray = [Int](0..<30)
    var hoursArray = [Int](0..<24)
    var minutesArray = [Int](0..<59)
    var secondsArray = [Int](0..<59)
    
    private let hoursInDay = 24
    private let secondsInMinute = 60
    private let minutesInHour = 60
    private let secondsInHour = 3600
    private let secondsInDay = 86400
    
    //@State private var daySelection = 0
    @State private var hourSelection = 0
    @State private var minuteSelection = 1
    @State private var secondSelection = 0
    
    //private let frameHeight: CGFloat = 160
    private let frameHeight: CGFloat = 160
    var body: some View {
        GeometryReader { geometry in
            //HStack(spacing: 0) {
            HStack(spacing: 0) {
                /*Picker(selection: self.$daySelection, label: Text("")) {
                    ForEach(0 ..< self.daysArray.count) { index in
                        Text("\(self.daysArray[index]) d").tag(index)
                    }
                }
                .onChange(of: self.daySelection) { newValue in
                    seconds = totalInSeconds
                }
                .frame(width: geometry.size.width/4, height: frameHeight, alignment: .center)
                .clipped()*/
                
                Picker(selection: self.$hourSelection, label: Text("")) {
                    ForEach(0 ..< self.hoursArray.count) { index in
                        Text("\(self.hoursArray[index]) h").tag(index)
                    }
                }//.pickerStyle(WheelPickerStyle())
                .onChange(of: self.hourSelection) { newValue in
                    seconds = totalInSeconds
                }.frame(width: geometry.size.width/3, alignment: .center)
                //.frame(width: geometry.size.width/3, height: frameHeight, alignment: .center)
                .clipped()
                
                Picker(selection: self.$minuteSelection, label: Text("")) {
                    ForEach(0 ..< self.minutesArray.count) { index in
                        Text("\(self.minutesArray[index]) m").tag(index)
                    }
                }
                .onChange(of: self.minuteSelection) { newValue in
                    seconds = totalInSeconds
                }
                .frame(width: geometry.size.width/3, alignment: .center)
                //.frame(width: geometry.size.width/3, height: frameHeight, alignment: .center)
                .clipped()
                
                Picker(selection: self.self.$secondSelection, label: Text("")) {
                    ForEach(0 ..< self.secondsArray.count) { index in
                        Text("\(self.secondsArray[index]) s").tag(index)
                    }
                }
                .onChange(of: self.secondSelection) { newValue in
                    seconds = totalInSeconds
                }
                .frame(width: geometry.size.width/3, alignment: .center)
                //.frame(width: geometry.size.width/3, height: frameHeight, alignment: .center)
                .clipped()
            }
        }
        .onAppear {
            updatePickers()
        }
    }
    
    func updatePickers() {
        /*//daySelection = seconds.secondsToDays
        hourSelection = seconds.secondsToHours
        minuteSelection = seconds.secondsToMinutes
        secondSelection = seconds.secondsRemainder
         */
        var seconds_temp = seconds
        secondSelection = seconds_temp%60
        seconds_temp -= secondSelection
        minuteSelection = (seconds_temp%3600)/60
        seconds_temp -= (minuteSelection*60)
        hourSelection = seconds_temp/3600
    }
    
    var totalInSeconds: Int {
        return hourSelection * self.secondsInHour + minuteSelection * self.secondsInMinute + secondSelection
    }
    
}
struct DatePickerView_Previews: PreviewProvider {

    
    static var timeSeconds: Int = 23

    
    static var previews: some View {
        DatePickerView(seconds: .constant(timeSeconds))
    }
}
