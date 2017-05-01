project("data")
 
set(DATA_DIR "${BIN_DIR}/data")
set(ARCHIVE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/archive")
file(MAKE_DIRECTORY ${DATA_DIR})
file(MAKE_DIRECTORY ${ARCHIVE_DIR})

# define data files
set(DATA_FILES
    uni.obj
    uni.obj.mtl
    GPS_InfoAndNMEALogging/T/NMEA_Log__2_47_25__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/T/NMEA_Log__2_48_20__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/T/NMEA_Log__2_49_19__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/T/NMEA_Log__2_50_13__Nachm.__03_28_2017.txt

    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__2_52_48__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__2_55_46__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__2_58_03__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__3_01_33__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__3_03_11__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__3_04_34__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__3_05_47__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__3_07_17__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__3_09_42__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__3_11_04__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__3_12_54__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__3_14_41__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__3_16_46__Nachm.__03_28_2017.txt
    GPS_InfoAndNMEALogging/Wegpunkte/NMEA_Log__3_17_52__Nachm.__03_28_2017.txt

    GPS_InfoAndNMEALogging/Planned/NMEA_Log__4_13_21__Nachm.__04_01_2017.txt
    GPS_InfoAndNMEALogging/Planned/NMEA_Log__4_26_03__Nachm.__04_01_2017.txt

    archive/2017-03-28.tle
    archive/2017-04-01.tle
)

# copy data files to data dictionary
foreach(data_file ${DATA_FILES})
    configure_file("${data_file}" "${DATA_DIR}/" COPYONLY)
endforeach(data_file)

# download current TLE data
file(DOWNLOAD "http://celestrak.com/NORAD/elements/gps-ops.txt" "${DATA_DIR}/gps-ops.tle.txt")
file(DOWNLOAD "http://celestrak.com/NORAD/elements/galileo.txt" "${DATA_DIR}/galileo.tle.txt")
file(DOWNLOAD "http://celestrak.com/NORAD/elements/glo-ops.txt" "${DATA_DIR}/glo-ops.tle.txt")


# concat TLE files into one TLE file
file(WRITE "${DATA_DIR}/current.tle" "")
append_file("${DATA_DIR}/gps-ops.tle.txt" "${DATA_DIR}/current.tle")
# append_file("${DATA_DIR}/galileo.tle.txt" "${DATA_DIR}/current.tle")
append_file("${DATA_DIR}/glo-ops.tle.txt" "${DATA_DIR}/current.tle")

string (TIMESTAMP current_date "%Y-%m-%d")
configure_file("${DATA_DIR}/current.tle" "${ARCHIVE_DIR}/${current_date}.tle" COPYONLY)
configure_file("${DATA_DIR}/gps-ops.tle.txt" "${ARCHIVE_DIR}/gps-ops.${current_date}.tle.txt" COPYONLY)
configure_file("${DATA_DIR}/galileo.tle.txt" "${ARCHIVE_DIR}/galileo.${current_date}.tle.txt" COPYONLY)
configure_file("${DATA_DIR}/glo-ops.tle.txt" "${ARCHIVE_DIR}/glo-ops.${current_date}.tle.txt" COPYONLY)

