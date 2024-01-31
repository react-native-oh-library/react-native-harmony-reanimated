import {Text, View, StyleSheet} from 'react-native';
import React from 'react';

export default function WaterfallGridExample() {
  return (
    <View style={styles.flexOne}>
      <Text>Picker is not supported on harmony</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  flexOne: {
    flex: 1,
  },
  pok: {
    alignItems: 'center',
    justifyContent: 'center',
    position: 'absolute',
  },
  pickerContainer: {
    flexDirection: 'row',
    justifyContent: 'center',
    height: 60,
    padding: 10,
  },
  picker: {
    height: 50,
    width: 250,
  },
});
