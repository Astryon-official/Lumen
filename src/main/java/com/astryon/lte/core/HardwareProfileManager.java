package com.astryon.lte.core;

import java.io.*;

public class HardwareProfileManager {

    private static final File FILE =
        new File("config/lte/hardware.profile");


    public static void save(HardwareProfile profile)
    {
        try {

            FILE.getParentFile().mkdirs();

            PrintWriter writer =
                new PrintWriter(FILE);

            writer.println(profile.cpuScore);
            writer.println(profile.gpuScore);
            writer.println(profile.backend);
            writer.println(profile.gpuName);

            writer.close();

            System.out.println(
                "[LTE] Hardware profile saved"
            );

        } catch(Exception e) {
            e.printStackTrace();
        }
    }


    public static HardwareProfile load()
    {
        try {

            if(!FILE.exists())
                return null;


            BufferedReader reader =
                new BufferedReader(
                    new FileReader(FILE)
                );


            long cpu =
                Long.parseLong(reader.readLine());

            long gpu =
                Long.parseLong(reader.readLine());

            String backend =
                reader.readLine();

            String gpuName =
                reader.readLine();


            reader.close();


            System.out.println(
                "[LTE] Hardware profile loaded"
            );


            return new HardwareProfile(
                cpu,
                gpu,
                backend,
                gpuName
            );


        } catch(Exception e) {

            return null;
        }
    }
}
