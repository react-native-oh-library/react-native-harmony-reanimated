import {View, StyleSheet, Text} from 'react-native';
import React from 'react';

export default function VolumeExample() {
  return (
    <View style={styles.container}>
      <Text>@react-native-community/slider not supported on harmony</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    flexDirection: 'column',
    padding: 20,
  },
});
